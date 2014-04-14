#include <stdlib.h>
#include <string.h>

#include "bagride2.h"
#include "mmtrace.h"
#include "qdirty.h"


QD_DECL_STR(html);
QD_DECL_STR(js);
QD_DECL_STR(ico);

static const br_http_type_item_t http_hrsr_items[] = {
    {
        .id = QD_STR(html),
        .response_type = "text/html"
    },
    {
        .id = QD_STR(js),
        .response_type = "text/javascript"
    },
    {
        .id = QD_STR(ico),
        .response_type = "text/html" /* TODO */
    },    
};

static void on_http_close(uv_handle_t* handle_) {
    br_http_client_t* cli = (br_http_client_t*) handle_->data;
    MM_INFO("(%5d) connection closed ", cli->m_request_num);
    free(cli->m_resbuf.base);
    free(cli);
}

static void on_http_after_write(uv_write_t* req_, int status_) {
    MM_ASSERT(status_ >= 0);
    uv_close((uv_handle_t*) req_->handle, on_http_close);
}


static int on_headers_complete(http_parser* parser) {
    br_http_client_t* cli = (br_http_client_t*) parser->data;
    MM_INFO("(%5d) http message parsed", cli->m_request_num);

    br_http_server_parser_cb response_cb = (br_http_server_parser_cb) cli->m_server->m_gen_response_cb;
    MM_ASSERT(0 == response_cb(cli));

    uv_write(
            &cli->m_write_req,
            (uv_stream_t*) & cli->m_handle,
            &cli->m_resbuf,
            1,
            on_http_after_write);
    return 0;
}

static int on_url_ready(http_parser* parser, const char *at, size_t length) {
    br_http_client_t* cli = (br_http_client_t*) parser->data;
    if (BR_MAX_REQ_URL_LEN <= length) MM_GERR("url too big");

    strncpy(cli->requested_url, at, length);
    cli->requested_url[length] = '\0';
    MM_INFO("(%5d) %s", cli->m_request_num, cli->requested_url);
    return 0;
err:
    return 1;
}

static void on_http_read(uv_stream_t* handle_, ssize_t nread_, const uv_buf_t* buf_) {
    size_t parsed;
    br_http_client_t* client = (br_http_client_t*) handle_->data;
    br_http_server_t* server = client->m_server;

    if (nread_ >= 0) {
        parsed = http_parser_execute(
                &client->m_parser, &server->m_parser_settings, buf_->base, (size_t) nread_);

        if (parsed < (size_t) nread_) {
            MM_ERR("parse error");
            uv_close((uv_handle_t*) handle_, on_http_close);
        }

    } else {
        uv_close((uv_handle_t*) handle_, on_http_close);
    }
    free(buf_->base);
}

static void on_http_connect(uv_stream_t* handle_, int status_) {
    MM_ASSERT(status_ >= 0);
    br_http_server_t* server = (br_http_server_t*) handle_->data;

    ++(server->m_request_num);

    br_http_client_t* cli = calloc(1, sizeof (br_http_client_t));
    cli->m_request_num = server->m_request_num;
    cli->m_server = server;

    MM_INFO("(%5d) connection new %p", cli->m_request_num, handle_);

    uv_tcp_init(uv_default_loop(), &cli->m_handle);
    http_parser_init(&cli->m_parser, HTTP_REQUEST);

    cli->m_parser.data = cli;
    cli->m_handle.data = cli;

    int r = uv_accept(handle_, (uv_stream_t*) & cli->m_handle);
    if (0 == r) {
        uv_read_start((uv_stream_t*) & cli->m_handle, on_alloc_buffer, on_http_read);
    } else {
        MM_ERR("connect:%s", uv_strerror(r));
        uv_close((uv_handle_t*) & cli->m_handle, on_http_close);
    }
}



int br_http_server_init(br_http_server_t* srv_, const br_http_srv_spec_t* spec_) {

    int res = 0;

    srv_->m_parser_settings.on_headers_complete = on_headers_complete;
    srv_->m_parser_settings.on_url = on_url_ready;
    srv_->m_port = spec_->m_port;
    srv_->m_gen_response_cb = spec_->m_gen_response_cb;
    MM_INFO("(%5d) http %ld", srv_->m_port);
    uv_tcp_init(uv_default_loop(), &srv_->m_handler);
    srv_->m_handler.data = srv_;
    MM_ASSERT(0 == uv_ip4_addr("0.0.0.0", srv_->m_port, &srv_->m_addr));
    MM_ASSERT(0 == uv_tcp_bind(&srv_->m_handler, (const struct sockaddr*) &srv_->m_addr));
    MM_ASSERT(0 == uv_listen((uv_stream_t*) & srv_->m_handler, BR_MAX_CONNECTIONS, on_http_connect));

    /* file type handling ... static list for now */
    {
        srv_->m_types = hashmap_new();

        size_t i;
        size_t sz = sizeof (http_hrsr_items) / sizeof (br_http_type_item_t);
        for (i = 0; i < sz; i++) {
            const br_http_type_item_t* p = &http_hrsr_items[i];
            if (MAP_OK != hashmap_put(srv_->m_types, p->id, (any_t) p))
                MM_GERR("resource add: %s %s", p->id, p->response_type);
        }
    }

    if (spec_->m_static_resources) {
        srv_->m_resources = hashmap_new();

        size_t i;
        for (i = 0; i < spec_->m_static_resources_sz; i++) {
            const mmembed_s* s = spec_->m_static_resources[i];
            MM_INFO("adding %s (%zu)", s->key, s->sz);
            if(0 > br_http_server_rsr_add(srv_, s->key, s->data, s->sz)) goto err;
        }
    }

end:

    return res;

err:
    res = -1;
    goto end;
}

int br_http_server_rsr_add(br_http_server_t* srv_, const char* key_,
        const unsigned char* data_, const size_t size_) {

    br_http_resource_t* rsr = malloc(sizeof (br_http_resource_t));
    if (!rsr) MM_GERR("resource container allocation");
    rsr->m_data = data_;
    rsr->m_len = size_;

    /* type extract from key suffix
     * -> search suffix from end 
     */
    const char* p = key_ + strlen(key_);
    const char* end = p;
    rsr->m_type = NULL;
    do {
        if ((*p == '.') && (p != end)) {
            rsr->m_type = p + 1;
            MM_INFO("added type: %s %s", key_, rsr->m_type);
            break;
        }
        --p;
    } while (p != key_);

    if (!rsr->m_type) MM_GERR("invalid resource: %d", key_);

    if (MAP_OK != hashmap_put(srv_->m_resources, key_, rsr)) MM_GERR("resource add: %d", key_);
    return 0;

err:

    return -1;
}





