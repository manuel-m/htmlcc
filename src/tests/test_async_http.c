#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mmtrace.h"
#include "bagride2.h"
#include "br_http.h"

static br_http_srv_t http_server;

int main(int argc, char **argv) {
    (void) argc;
    (void) argv;
    int res = 0;

    br_http_srv_spec_t http_srv_spec = {
        .m_port = 9999,
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