#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mmtrace.h"
#include "bagride2.h"


extern const char mini_html[];
extern const size_t mini_html_len;
extern const char not_found_html[];
extern const size_t not_found_html_len;

static br_http_server_t srv;

static int on_stats_response(br_http_client_t* c_) {

    br_http_resource_t* rsr;

    if (MAP_OK != (hashmap_get(c_->m_server->m_resources, c_->requested_url, (void**) (&rsr)))) {
        MM_INFO("(%5d) invalid requested url:%s", c_->m_request_num, c_->requested_url);
        hashmap_get(c_->m_server->m_resources, "not_found", (void**) (&rsr));
    }

    c_->m_resbuf.len = asprintf(&c_->m_resbuf.base,
            "HTTP/1.1 200 OK\r\n"
            "Server: cc/0.1\r\n"
            "Content-Type: text/html\r\n"
            "Connection: close\r\n"
            "Content-Length: %d\r\n"
            "\r\n"
            "%s",
            (int) rsr->m_len, rsr->m_data);
    return 0;
}

int main(int argc, char **argv) {
    (void) argc;
    (void) argv;

    br_http_server_init(&srv, 9999, on_stats_response);
    br_http_server_resource_add(&srv, "/", mini_html, mini_html_len);
    br_http_server_resource_add(&srv, "not_found", not_found_html, not_found_html_len);


    br_run();
    return 0;
}