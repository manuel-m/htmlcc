#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mmtrace.h"
#include "bagride2.h"

static br_http_srv_t http_server;

static int on_stats_response(br_http_cli_t* c_) {

    c_->m_resbuf.len = asprintf(&c_->m_resbuf.base, "%s",
            "HTTP/1.1 200 OK\n"
            "Date: Thu, 19 Feb 2009 12:27:04 GMT\n"
            "Server: cc/0.1\n"
            "Last-Modified: Wed, 18 Jun 2003 16:05:58 GMT\n"
            "ETag: \"56d-9989200-1132c580\"\n"
            "Content-Type: text/html\n"
            "Accept-Ranges: bytes\n"
            "Connection: close\n"
            "Content-Length: 15\n"
            "\n"
            "sdfkjsdnbfkjbsf");
    return 0;
}

int main(int argc, char **argv) {
    (void) argc;
    (void) argv;
    int res = 0;

    br_http_srv_spec_t http_srv_spec = {
        .m_port = 9999,
        .m_gen_response_cb = on_stats_response,
        .m_static_resources_sz = 0,
        .m_static_resources = NULL
    };

    if (0 > br_http_srv_init(&http_server, &http_srv_spec)) goto err;
    br_run();

end:
    return res;

err:
    res = -1;
    goto end;
}