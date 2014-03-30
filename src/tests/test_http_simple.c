#include <stdio.h> 
#include <stdlib.h> 
#include <errno.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <unistd.h>
#include <arpa/inet.h>

int main(void) {

    char *reply =
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

    int sd = socket(PF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr;
    bzero(&addr, sizeof (addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8081);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sd, &addr, sizeof (addr)) != 0) {
        printf("bind error\n");
    }

    if (listen(sd, 16) != 0) {
        printf("listen error\n");
    }

    for (;;) {
        socklen_t size = sizeof (addr);
        int client = accept(sd, &addr, &size);
        
        if (client > 0) {
            printf("client connected\n");
            send(client, reply, strlen (reply), 0);
        }
    }

    return 0;
}