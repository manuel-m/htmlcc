#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mmtrace.h"
#include "bagride2.h"
#include "br_http.h"
#include "sub0.h"

#include "hxd_tinycontrol.h"

static br_http_srv_t srv;


const char rsr_404[] = "/404.html";

int main(int argc, char **argv) {
    (void) argc;
    (void) argv;
    int res = 0;

    const br_http_srv_spec_t http_srv_spec = {
        .m_port = 9999,
        .m_gen_response_cb = on_stats_response_generic,
        .m_static_resources = &rsr_tinycontrol,
        .m_rsr_404 = rsr_404
    };

    if (0 > br_http_srv_init(&srv, &http_srv_spec)) goto err;

    br_run();

end:
    return res;

err:
    res = -1;
    goto end;

}