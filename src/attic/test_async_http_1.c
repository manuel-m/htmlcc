#include <stdio.h> 
#include <stdlib.h> 
#include <errno.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <unistd.h>
#include <arpa/inet.h>

#define MYPORT 10102  
#define BACKLOG 5   
#define MAXCLIENTS 5
#define QDCLIENT_BUF_SIZE (1<<16)

#define QDERR(MERROR)                                                          \
do {                                                                           \
res = -1;                                                                      \
char* bufferr;                                                                 \
if(0 < asprintf(&bufferr,"%s,%s,%d,", #MERROR, __FILE__, __LINE__)){           \
  perror(bufferr);                                                             \
  free(bufferr);}                                                              \
goto end;                                                                      \
}while(0);

#define MODES_CONTENT_TYPE_HTML "text/html;charset=utf-8"
#define MODES_CONTENT_TYPE_JSON "application/json;charset=utf-8"

#define QDMINIBODY "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"utf-8\"><title>async http test</title></head><body>test</body></html>"

/* Structure used to describe a networking client. */
struct client {
    int fd; /* File descriptor. */
    char buf[QDCLIENT_BUF_SIZE + 1]; /* Read buffer. */
    int buflen; /* Amount of data on buffer. */
};

int main(void) {
    int res = 0;
    int sockfd, new_fd, numbytes, highest = 0, i;

    struct client clients[MAXCLIENTS];
    memset(clients, 0, sizeof (clients));

    struct sockaddr_in my_addr, their_addr;
    socklen_t sin_size;
    struct timeval tv;
    fd_set readfds;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) QDERR(socket);
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(MYPORT);
    my_addr.sin_addr.s_addr = INADDR_ANY;
    memset(&(my_addr.sin_zero), 0, 8);

    if (bind(sockfd, (struct sockaddr*) &my_addr, sizeof (struct sockaddr)) < 0) QDERR(socket);
    if (listen(sockfd, BACKLOG) < 0) QDERR(listen);

    memset(clients, 0, sizeof (clients));
    highest = sockfd;
    do {
        sin_size = sizeof (struct sockaddr_in);
        tv.tv_sec = 0;
        tv.tv_usec = 250000;
        FD_ZERO(&readfds);
        for (i = 0; i < MAXCLIENTS; i++) if (clients[i].fd) FD_SET(clients[i].fd, &readfds);

        FD_SET(sockfd, &readfds);
        if (select(highest + 1, &readfds, NULL, NULL, &tv) >= 0) {
            if (FD_ISSET(sockfd, &readfds)) {
                if ((new_fd = accept(sockfd, (struct sockaddr *) &their_addr, &sin_size)) < 0) {
                    perror("accept");
                    continue;
                }
                for (i = 0; i < MAXCLIENTS; i++) {
                    if (clients[i].fd == 0) {
                        clients[i].fd = new_fd;
                        break;
                    }
                }
                if (i != MAXCLIENTS) {
                    if (new_fd > highest) {
                        highest = clients[i].fd;
                    }
                    printf("Connexion received from %s (slot %i)\n", inet_ntoa(their_addr.sin_addr), i);
                    send(new_fd, "Welcome !", sizeof ("Welcome !"), MSG_NOSIGNAL);
                } else {
                    send(new_fd, "No room for you !\n", sizeof ("No room for you !\n"), MSG_NOSIGNAL);
                    close(new_fd);
                }
            }
            for (i = 0; i < MAXCLIENTS; i++) {
                if (FD_ISSET(clients[i].fd, &readfds)) {
                    if ((numbytes = recv(clients[i].fd, clients[i].buf, QDCLIENT_BUF_SIZE, 0)) <= 0) {
                        printf("Connexion lost from slot %i\n", i);
                        close(clients[i].fd);
                        clients[i].fd = 0;
                    } else {
                        clients[i].buf[numbytes] = '\0';
                        printf("Received from slot %i : %s", i, clients[i].buf);

                        char reply[] =
                                "HTTP/1.1 200 OK\n"
                                "Date: Thu, 19 Feb 2009 12:27:04 GMT\n"
                                "Server: Apache/2.2.3\n"
                                "Last-Modified: Wed, 18 Jun 2003 16:05:58 GMT\n"
                                "ETag: \"56d-9989200-1132c580\"\n"
                                "Content-Type: text/html\n"
                                "Content-Length: 15\n"
                                "Accept-Ranges: bytes\n"
                                "Connection: close\n"
                                "\n"
                                "sdfkjsdnbfkjbsf";

                        fprintf(stdout, "%s\n", reply);

                        send(clients[i].fd, reply, strlen(reply), MSG_NOSIGNAL);


                    }



                }
            }
        } else {
            perror("select");
            continue;
        }
    } while (1);


end:
    return (res);

}