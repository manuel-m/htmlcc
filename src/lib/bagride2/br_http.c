#include <stdlib.h>
#include <string.h>

#include "br_alloc.h"
#include "br_http.h"
#include "mmtrace.h"
#include "qdirty.h"
#include "sub0.h"
#include "favicon.h"
#include "strcompat.h"


/* Request Methods */
#define BR_STATIC_RSR_MAP(XX)                                                  \
    XX(html, "text/html")                                                      \
    XX(js, "text/javascript")                                                  \
    XX(ico, "image/x-icon")                                                    \
    XX(png, "image/png")                                                       \
    XX(css, "text/css")                                                        \
    XX(jpg, "image/jpeg")                                                      \
    XX(gif, "image/gif")  

#define XX(name, response) static const char name##_str[] = # name;
  BR_STATIC_RSR_MAP(XX)
#undef XX

#define BR_HTML_HEADER_TYPE(BR_TYPENAME, BR_TYPESTRING)\
 {.id=QD_STR(BR_TYPENAME),.response_type=BR_TYPESTRING}

static const br_http_type_item_t http_hrsr_items[] = {
      
#define XX(name, response) {.id=name##_str,.response_type=response},
  BR_STATIC_RSR_MAP(XX)
#undef XX      
          
};

#undef BR_STATIC_RSR_MAP

static void on_http_close(uv_handle_t* handle_) {
    br_http_cli_t* cli = (br_http_cli_t*) handle_->data;
    MM_INFO("(%5d) connection closed ", cli->m_request_num);
    free(cli->m_resbuf.base);
    free(cli);
}

static void on_http_after_write(uv_write_t* req_, int status_) {
    MM_ASSERT(status_ >= 0);
    uv_close((uv_handle_t*) req_->handle, on_http_close);
}

static int on_body(http_parser *parser_, const char *p_, size_t sz_) {
    br_http_cli_t* cli = (br_http_cli_t*) parser_->data;
    char buf[2048];
    memcpy(buf, p_, (sz_ < (sizeof (buf))) ? sz_ : (sizeof (buf) - 1));
    buf[sz_] = '\0';
    MM_INFO("(%5d) body_size,%d ", cli->m_request_num, sz_);
    MM_INFO("(%5d) body,%s ", cli->m_request_num, buf);
    return 0;
}

#define BR_HTTP_GET_m                                                          \
    br_http_cli_t* cli = (br_http_cli_t*) p_->data;                            \
    struct message_s *m = &cli->pub.m_mess;

static int request_url_cb(http_parser *p_, const char *buf_, size_t sz_) {
    BR_HTTP_GET_m
    strlncat(m->request_url, sizeof (m->request_url), buf_, sz_);
    MM_INFO("(%5d) %s", cli->m_request_num, cli->pub.m_mess.request_url);
    return 0;
}

static int header_field_cb(http_parser *p_, const char *buf_, size_t sz_) {
    BR_HTTP_GET_m
    if (m->last_header_element != BR_HEADER_FIELD) m->num_headers++;
    strlncat(m->headers[m->num_headers - 1][0], sizeof (m->headers[m->num_headers - 1][0]), buf_,   sz_);
    m->last_header_element = BR_HEADER_FIELD;
    return 0;
}

static int header_value_cb(http_parser *p_, const char *buf_, size_t sz_) {
    BR_HTTP_GET_m
    strlncat(m->headers[m->num_headers - 1][1],
            sizeof (m->headers[m->num_headers - 1][1]), buf_, sz_);
    m->last_header_element = BR_HEADER_VALUE;
    return 0;
}

static int message_begin_cb(http_parser *p_) {
    BR_HTTP_GET_m
    m->message_begin_cb_called = 1;
    return 0;
}

static int on_headers_complete(http_parser* p_) {
    BR_HTTP_GET_m
    m->method = p_->method;
    m->status_code = p_->status_code;
    m->http_major = p_->http_major;
    m->http_minor = p_->http_minor;
    m->headers_complete_cb_called = 1;
    m->should_keep_alive = http_should_keep_alive(p_);
    return 0;
}

static int message_complete_cb(http_parser* p_) {
    BR_HTTP_GET_m
    MM_INFO("(%5d) http_message_complete", cli->m_request_num);

    br_http_srv_parser_cb response_cb = (br_http_srv_parser_cb) cli->m_srv->m_gen_response_cb;
    MM_ASSERT(0 == response_cb(cli));

    uv_write(
            &cli->m_write_req,
            (uv_stream_t*) & cli->m_handle,
            &cli->m_resbuf,
            1,
            on_http_after_write);
    
    m->message_complete_cb_called = 1;
    return 0;
}

static int response_status_cb (http_parser *p_, const char *buf_, size_t sz_)
{
    BR_HTTP_GET_m
    strlncat(m->response_status, sizeof(m->response_status), buf_, sz_);
  return 0;
}

static void on_http_read(uv_stream_t* handle_, ssize_t nread_, const uv_buf_t* buf_) {
    
    size_t parsed;
    br_http_cli_t* cli = (br_http_cli_t*) handle_->data;
    br_http_srv_t* srv = cli->m_srv;

    if (nread_ >= 0) {
        parsed = http_parser_execute(
                &cli->m_parser, &srv->m_parser_settings, buf_->base, (size_t) nread_);

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
    br_http_srv_t* server = (br_http_srv_t*) handle_->data;

    ++(server->m_request_num);

    br_http_cli_t* cli = calloc(1, sizeof (br_http_cli_t));
    cli->m_request_num = server->m_request_num;
    cli->m_srv = server;

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


int br_http_srv_init(br_http_srv_t* srv_, const br_http_srv_spec_t* spec_) {

    int res = 0;
    http_parser_settings *settings =  &srv_->m_parser_settings;
    
    settings->on_header_field = header_field_cb;
    settings->on_header_value = header_value_cb;
    settings->on_url = request_url_cb;
    settings->on_message_begin = message_begin_cb;
    settings->on_status = response_status_cb;
    
    settings->on_headers_complete = on_headers_complete;
    settings->on_message_complete = message_complete_cb;
    
    settings->on_body = on_body;
    
    srv_->m_port = spec_->m_port;
    srv_->m_gen_response_cb = spec_->m_gen_response_cb;
    srv_->m_rsr_404 = spec_->m_rsr_404;
    MM_INFO("(%5d) http %ld", srv_->m_port);
    uv_tcp_init(uv_default_loop(), &srv_->m_handler);
    srv_->m_handler.data = srv_;
    MM_ASSERT(0 == uv_ip4_addr("0.0.0.0", srv_->m_port, &srv_->m_addr));
    MM_ASSERT(0 == uv_tcp_bind(&srv_->m_handler, (const struct sockaddr*) &srv_->m_addr));
    MM_ASSERT(0 == uv_listen((uv_stream_t*) & srv_->m_handler, BR_MAX_CONNECTIONS, on_http_connect));

    if (NULL == spec_->m_rsr_404) MM_GERR("missing 404 definition");

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
        for (i = 0; i < spec_->m_static_resources->m_sz; i++) {
            const rsr_t* prsr = spec_->m_static_resources->m_array[i];
            MM_INFO("adding %s (%zu)", prsr->m_key, prsr->m_sz);
            if (0 > br_http_srv_static_rsr_add(srv_, prsr)) goto err;
        }

        /* favicon.ico 
         * -> we add if not defined */
        {
            const rsr_t* prsr = NULL;
            hashmap_get(srv_->m_resources, rsr_favicon.m_key, (void**) (&prsr));
            if (!prsr) if (0 > br_http_srv_static_rsr_add(srv_, &rsr_favicon)) goto err;
        }
    }

end:

    return res;

err:
    res = -1;
    goto end;
}

int br_http_srv_static_rsr_add(br_http_srv_t* srv_, const rsr_t* rsr_) {

    const char* suffix = sub0_path_suffix(rsr_->m_key);
    if (NULL == suffix) MM_GERR("static resource without suffix %s", rsr_->m_key);

    /* check if type is handled */
    br_http_type_item_t* type = NULL;
    if (MAP_OK != (hashmap_get(srv_->m_types, suffix, (void**) (&type))))
        MM_GERR("static resource not handled (suffix) %s (%s)", rsr_->m_key);


    if (MAP_OK != hashmap_put(srv_->m_resources, rsr_->m_key, (any_t) rsr_))
        MM_GERR("resource add: %s", rsr_->m_key);
    return 0;

err:
    return -1;
}

int on_stats_response_generic(br_http_cli_t* c_) {

    rsr_t* rsr;
    const char index_url[] = "/index.html";
    const char* url = c_->pub.m_mess.request_url;

    const br_http_srv_t* srv = c_->m_srv;

    /* '/' =>  '/index.html' */
    if (0 == strcmp(url, "/")) url = index_url;

    if (MAP_OK != (hashmap_get(srv->m_resources, url, (void**) (&rsr)))) {
        MM_INFO("(%5d) invalid requested url:%s", c_->m_request_num, url);
        /* not found 404 fallback*/
        hashmap_get(srv->m_resources, srv->m_rsr_404, (void**) (&rsr));
    }
    const char* suffix = sub0_path_suffix(url);
    br_http_type_item_t* type = NULL;

    if (suffix) {
        if (MAP_OK != (hashmap_get(srv->m_types, suffix, (void**) (&type)))) {
            MM_INFO("(%5d) invalid requested type:%s", c_->m_request_num, url);
        }
    }
    /* fallback ... no type detected ... => html */
    if (NULL == type) hashmap_get(srv->m_types, "html", (void**) (&type));

    MM_INFO("(%5d) response type:%s %s", c_->m_request_num, type->id, type->response_type);

    char* header_value = NULL;
    size_t header_value_sz = 0;

    header_value_sz = asprintf(&header_value,
            "HTTP/1.1 200 OK\r\n"
            "Server: htmlcc/0.1\r\n"
            "Content-Type: %s\r\n"
            "X-Content-Type-Options: nosniff \r\n"
            "Connection: close\r\n"
            "Content-Length: %zu\r\n"
            "\r\n",
            type->response_type,
            rsr->m_sz);

    /**
     * TODO
     * the intermediated malloc shall be avoided
     */
    c_->m_resbuf.len = rsr->m_sz + header_value_sz;
    c_->m_resbuf.base = malloc(c_->m_resbuf.len);
    strcpy(c_->m_resbuf.base, header_value);
    free(header_value);

    memcpy(c_->m_resbuf.base + header_value_sz, rsr->m_data, rsr->m_sz);

    return 0;
}



