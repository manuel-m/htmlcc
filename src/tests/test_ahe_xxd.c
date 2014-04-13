#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mmtrace.h"
#include "bagride2.h"

#include "htmlcc.html.c"
#include "not_found.html.c"
#include "remark.min.js.c"

static br_http_server_t srv;

static int on_stats_response(br_http_client_t* c_) {

    br_http_resource_t* rsr;

    if (MAP_OK != (hashmap_get(c_->m_server->m_resources, c_->requested_url, (void**) (&rsr)))) {
        MM_INFO("(%5d) invalid requested url:%s", c_->m_request_num, c_->requested_url);
        hashmap_get(c_->m_server->m_resources, "not_found", (void**) (&rsr));
    }

    c_->m_resbuf.len = asprintf(&c_->m_resbuf.base,
            "HTTP/1.1 200 OK\r\n"
            "Server: htmlcc/0.1\r\n"
            "Content-Type: %s\r\n"
            "Connection: close\r\n"
            "Content-Length: %d\r\n"
            "\r\n"
            "%s",
            BR_HTML == rsr->type ? "text/html" : "text/javascript",
            (int) rsr->m_len,
            rsr->m_data);

    return 0;
}

int main(int argc, char **argv) {
    (void) argc;
    (void) argv;
    
    br_http_server_init(&srv, 9999, on_stats_response);
    
    
    br_http_server_resource_add(&srv, "/", __htmlcc_html, __htmlcc_html_len, BR_HTML);
    br_http_server_resource_add(&srv, "/js/remark.min.js", __js_remark_min_js, __js_remark_min_js_len, BR_JS);
    br_http_server_resource_add(&srv, "not_found", __not_found_html, __not_found_html_len, BR_HTML);


    br_run();
    return 0;
}