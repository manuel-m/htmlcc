#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mmtrace.h"
#include "bagride2.h"

extern uint8_t js_remark_min_js[] asm("_binary___js_remark_min_js_start");
extern uint8_t js_remark_min_js_size[] asm("_binary___js_remark_min_js_size");
extern uint8_t js_remark_min_js_end[] asm("_binary___js_remark_min_js_end");

extern uint8_t not_found_html[] asm("_binary___not_found_html_start");
extern uint8_t not_found_html_size[] asm("_binary___not_found_html_size");
extern uint8_t not_found_html_end[] asm("_binary___not_found_html_end");

extern uint8_t htmlcc_html[] asm("_binary___htmlcc_html_start");
extern uint8_t htmlcc_html_size[] asm("_binary___htmlcc_html_size");
extern uint8_t htmlcc_html_end[] asm("_binary___htmlcc_html_end");

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
    
    const size_t htmlcc_html_sz =(size_t)((void*)htmlcc_html_size);
    const size_t js_remark_min_js_sz =(size_t)((void*)js_remark_min_js_size);
    const size_t not_found_html_sz =(size_t)((void*)not_found_html_size);

    br_http_server_init(&srv, 9999, on_stats_response);
    br_http_server_resource_add(&srv, "/", htmlcc_html, htmlcc_html_sz, BR_HTML);
    br_http_server_resource_add(&srv, "/js/remark.min.js", js_remark_min_js, js_remark_min_js_sz, BR_JS);
    br_http_server_resource_add(&srv, "not_found", not_found_html, not_found_html_sz, BR_HTML);


    br_run();
    return 0;
}