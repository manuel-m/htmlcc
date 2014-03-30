#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mmtrace.h"
#include "bagride.h"

extern const char mini_html[];
extern const size_t mini_html_len;

static br_http_server_t http_server;
static int on_stats_response(br_http_client_t* c_) {
    
    c_->m_resbuf.len = asprintf(&c_->m_resbuf.base, 
            "HTTP/1.1 200 OK\r\n"
            "Server: cc/0.1\r\n"
            "Content-Type: text/html\r\n"
            "Connection: close\r\n"            
            "Content-Length: %d\r\n"
            "\r\n"
            "%s", 
            (int)mini_html_len, mini_html);
    return 0;
}

int main(int argc, char **argv) {
    (void) argc;
    (void) argv;

    br_http_server_init(&http_server, 9999, on_stats_response);
    br_run();
    return 0;
}