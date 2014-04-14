#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mmtrace.h"
#include "bagride2.h"
#include "sub0.h"

#include "hxd_generated.h"

static br_http_srv_t srv;



static int on_stats_response(br_http_cli_t* c_) {

    rsr_t* rsr;
    const char index_url[] = "/index.html";
    const char* url = c_->requested_url;

    const br_http_srv_t* srv = c_->m_srv;

    /* '/' =>  '/index.html' */
    if (0 == strcmp(c_->requested_url, "/")) url = index_url;

    if (MAP_OK != (hashmap_get(srv->m_resources, url, (void**) (&rsr)))) {
        MM_INFO("(%5d) invalid requested url:%s", c_->m_request_num, url);
        hashmap_get(srv->m_resources, "/not_found.html", (void**) (&rsr));
    }
    const char* suffix = sub0_path_suffix(url);
    br_http_type_item_t* type = NULL;
    
    if (suffix) {
        if (MAP_OK != (hashmap_get(srv->m_types, suffix, (void**) (&type)))) {
            MM_INFO("(%5d) invalid requested type:%s", c_->m_request_num, url);
        }
    }
    /* fallback ... no type detected ... => html */
    if(NULL == type) hashmap_get(srv->m_types, "html", (void**) (&type));

    MM_INFO("(%5d) response type:%s %s", c_->m_request_num, type->id, type->response_type);
    
    c_->m_resbuf.len = asprintf(&c_->m_resbuf.base,
            "HTTP/1.1 200 OK\r\n"
            "Server: htmlcc/0.1\r\n"
            "Content-Type: %s\r\n"
            "Connection: close\r\n"
            "Content-Length: %zu\r\n"
            "\r\n"
            "%s",
            type->response_type,
            rsr->m_sz,
            rsr->m_data);


    return 0;
}

int main(int argc, char **argv) {
    (void) argc;
    (void) argv;

    const br_http_srv_spec_t http_srv_spec = {
        .m_port = 9999,
        .m_gen_response_cb = on_stats_response,
        .m_static_resources_sz = hxds_ahe_sz,
        .m_static_resources = hxds_ahe
    };

    br_http_srv_init(&srv, &http_srv_spec);

    br_run();
    return 0;
}