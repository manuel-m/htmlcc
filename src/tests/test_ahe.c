#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mmtrace.h"
#include "bagride2.h"
#include "sub0.h"

#include "hxd_generated.h"

static br_http_srv_t srv;


const char rsr_404[]="/404.html";


int main(int argc, char **argv) {
    (void) argc;
    (void) argv;

    const br_http_srv_spec_t http_srv_spec = {
        .m_port = 9999,
        .m_gen_response_cb = on_stats_response_generic,
        .m_static_resources_sz = hxds_ahe_sz,
        .m_static_resources = hxds_ahe,
        .m_rsr_404 = rsr_404
    };

    br_http_srv_init(&srv, &http_srv_spec);

    br_run();
    return 0;
}