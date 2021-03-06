#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "bagride2.h"
#include "br_alloc.h"
#include "sub0.h"
#include "mmtrace.h"


static int br_isipv4(const char *ip_, size_t size_) {
    int i;
    long int j;
    const char *start;
    char *end;

    for (i = 0, start = ip_; i < 4; i++, start = end + 1) {
        if (!isdigit((int) (unsigned char) *start)) return 0;
        errno = 0;
        j = strtol(start, &end, 10);
        if (j > 255) return 0;
        if (errno != 0 || j < 0 || j > 255) return 0;
    }
    if (i < 4 || end != ip_ + size_) return 0;
    return 1;
}

void br_tcp_server_close(br_tcp_server_t* srv_) {
    mmpool_free(srv_->m_clients);
}



static void on_close(uv_handle_t* client_handle_) {

    mmpool_item_t* client_pool_item = (mmpool_item_t*) client_handle_->data;
    br_tcp_server_t* server = (br_tcp_server_t*) client_pool_item->parent->userdata;

    mmpool_giveback(client_pool_item);

    log_info("%s:%d disconnect (%d/%d) (%p)",
            server->m_name,
            server->m_port,
            server->m_clients->taken_count,
            server->m_clients->capacity,
            client_handle_);
}

static void on_close_quick(uv_handle_t* client_handle_) {
    free(client_handle_);
}

static void on_after_write(uv_write_t* req_, int status_) {
    if(0 > status_ ) die("after write");
    char *base = (char*) req_->data;
    free(base);
    free(req_);
}

static void on_tcp_read(uv_stream_t* stream_, ssize_t nread_, const uv_buf_t* read_buf_) {
    if (nread_ > 0) {
        log_info("on_tcp_read (%p)", stream_);

        mmpool_item_t* client_pool_item = (mmpool_item_t*) stream_->data;
        br_tcp_server_t* tcp_server = (br_tcp_server_t*) client_pool_item->parent->userdata;
        br_buf_t* write_buffer = &tcp_server->m_write_buffer;
        br_tcp_server_parser_cb user_parse_cb = (br_tcp_server_parser_cb) tcp_server->m_user_parse_cb;
        if(0 != user_parse_cb(nread_, read_buf_, tcp_server))die_internal();
        if (write_buffer->len) {
            uv_write_t *req = (uv_write_t *) calloc(1, sizeof (uv_write_t));
            req->data = (void*) write_buffer->base;
            uv_write(req, stream_, (uv_buf_t*) write_buffer, 1, on_after_write);
        }
    } else {
        uv_close((uv_handle_t*) stream_, on_close);
    }
    free(read_buf_->base);
}

static void on_in_tcp_client_close(uv_handle_t* handle) {
    (void) handle;
}

static void on_in_tcp_client_read(uv_stream_t* stream_, ssize_t nread_, const uv_buf_t* buf_) {
    if (nread_ >= 0) {
        br_tcp_client_t* client = (br_tcp_client_t*) stream_->data;
        br_tcp_client_parser_cb user_cb = (br_tcp_client_parser_cb) client->m_user_parse_cb;
        if(0 != user_cb(nread_, buf_, client))die_internal();

    } else {
        uv_close((uv_handle_t*) stream_, on_in_tcp_client_close);
    }
    free(buf_->base);
}

int br_out_tcp_write_string(br_tcp_server_t* server_, const char* str_, size_t len_) {

    mmpool_iter_t iter = {
        .m_index = 0,
        .m_pool = server_->m_clients
    };

    uv_stream_t* pclient = (uv_stream_t*) mmpool_iter_next(&iter);

    while (pclient) {
        br_buf_t* write_buffer = &server_->m_write_buffer;
        write_buffer->len = len_;
        write_buffer->base = strdup(str_);
        uv_write_t *req = (uv_write_t *) calloc(1, sizeof (uv_write_t));
        req->data = (void*) write_buffer->base;
        uv_write(req, pclient, (uv_buf_t*) write_buffer, 1, on_after_write);
        pclient = (uv_stream_t*) mmpool_iter_next(&iter);
    }
    return 0;
}

static void on_in_udp_recv(uv_udp_t* stream_, ssize_t nread_, const uv_buf_t* read_buff_,
        const struct sockaddr* addr_, unsigned flags_) {

    (void) addr_;
    (void) flags_;

    if (nread_ > 0) {
        br_udp_server_t* server_udp = (br_udp_server_t*) stream_->data;
        br_udp_server_parser_cb user_parse_cb = (br_udp_server_parser_cb) server_udp->m_user_parse_cb;
        if(0 != user_parse_cb(nread_, read_buff_, server_udp))die_internal();
    }
    free(read_buff_->base);
}

static void on_udp_send(uv_udp_send_t* req_, int status) {
    (void) status;
    char *base = (char*) req_->data;
    free(base);
    free(req_);
}

static void on_tcp_client_connect(uv_connect_t* connection, int status) {
    (void) status;
    uv_stream_t* stream = connection->handle;
    uv_read_start(stream, on_alloc_buffer, on_in_tcp_client_read);
}

int br_tcp_client_init(br_tcp_client_t* client_, const char* name_,
        const char* addr_, int port_, void* user_parse_cb_) {
    strncpy(client_->m_name, name_, sizeof (client_->m_name));
    client_->m_port = port_;
    client_->m_user_parse_cb = user_parse_cb_;
    strncpy(client_->m_addr, addr_, sizeof (client_->m_addr));
    client_->m_handler.data = client_;

    uv_tcp_init(uv_default_loop(), &client_->m_handler);

    int r = uv_ip4_addr(addr_, port_, &client_->m_socketaddr);
    if (r != 0) {
        log_err("invalid address %s:", addr_);
        return -1;
    }
    r = uv_tcp_connect(&client_->m_connect, &client_->m_handler,
            (const struct sockaddr*) &client_->m_socketaddr,
            on_tcp_client_connect);
    if (r != 0) {
        log_err("failed connect %s:%d", addr_, port_);
        return -1;
    }
    return 0;
}

static void on_in_server_connect(uv_stream_t* server_handle_, int status_) {

    if(status_ < 0)die_internal();
    br_tcp_server_t* server = (br_tcp_server_t*) server_handle_->data;
    mmpool_item_t* client_pool_item = mmpool_take(server->m_clients);

    uv_tcp_t *pclient;

    if (NULL == client_pool_item) {
        log_warn("%s:%d max connections reached (%d) ... system wont accept new connections",
                server->m_name,
                server->m_port,
                server->m_clients->capacity);
        /* workaround : connection is accepted and immediatly closed.
         This allow to re-accept new connections after some disconnections ...*/
        pclient = malloc(sizeof (uv_tcp_t));
        uv_tcp_init(uv_default_loop(), pclient);
        uv_accept(server_handle_, (uv_stream_t*) pclient);
        uv_close((uv_handle_t*) pclient, on_close_quick);
        return;

    }
    pclient = (uv_tcp_t *) client_pool_item->p;

    uv_tcp_init(uv_default_loop(), pclient);
    pclient->data = client_pool_item;

    int r = uv_accept(server_handle_, (uv_stream_t*) pclient);

    if (0 == r) {
        log_info("%s:%d connect (%d/%d) (serv:%p cli:%p)",
                server->m_name,
                server->m_port,
                server->m_clients->taken_count,
                server->m_clients->capacity,
                server_handle_, pclient);
        uv_read_start((uv_stream_t*) pclient, on_alloc_buffer, on_tcp_read);
    } else {
        log_err("connection failed:%s", uv_strerror(r));
        uv_close((uv_handle_t*) pclient, on_close);
    }
}

int br_udp_server_init(br_udp_server_t* srv_, int port_, void* user_parse_cb_) {

    srv_->m_port = port_;
    srv_->m_user_parse_cb = user_parse_cb_;
    log_info("udp in %d", srv_->m_port);

    int r = uv_udp_init(uv_default_loop(), &srv_->m_handler);
    if (0 != r) {
        log_err("udp server init failed: %s", uv_strerror(r));
        return -1;
    }
    srv_->m_handler.data = srv_;
    if(0 != uv_ip4_addr("0.0.0.0", srv_->m_port, &srv_->m_socketaddr))die_internal();
    if(0 != uv_udp_bind(&srv_->m_handler, (const struct sockaddr*) &srv_->m_socketaddr, 0))die_internal();
    if(0 != uv_udp_recv_start(&srv_->m_handler, on_alloc_buffer, on_in_udp_recv))die_internal();
    return 0;
}

int br_tcp_server_init(br_tcp_server_t* server_, const char* name_, int port_,
        void* user_parse_cb_, int max_connections_) {

    strncpy(server_->m_name, name_, sizeof (server_->m_name));
    server_->m_port = port_;
    server_->m_user_parse_cb = user_parse_cb_;
    log_info("%s:%d server listening", server_->m_name, server_->m_port);
    uv_tcp_init(uv_default_loop(), &server_->m_server_handler);
    server_->m_server_handler.data = server_;

    /* clients */
    server_->m_clients = mmpool_easy_new(max_connections_, sizeof (br_tcp_t), server_);

    if(0 != uv_ip4_addr("0.0.0.0", server_->m_port, &server_->m_socketaddr))die_internal();
    if(0 != uv_tcp_bind(&server_->m_server_handler, (const struct sockaddr*) &server_->m_socketaddr))die_internal();
    if(0 != uv_listen((uv_stream_t*) & server_->m_server_handler, max_connections_, on_in_server_connect))die_internal();
    return 0;
}

static void on_resolved_udp_client(uv_getaddrinfo_t *resolver_, int status_,
        struct addrinfo *res_) {

    mmpool_item_t* cli_pool_item = (mmpool_item_t*) resolver_->data;
    br_udp_client_t* cli = (br_udp_client_t*) cli_pool_item->p;
    if (0 > status_) {
        log_err("dns failed for:%s %s", cli->m_addr, uv_strerror(status_));
        mmpool_giveback(cli_pool_item);
        return;
    } else {
        char ip_addr[17] = {'\0'};
        uv_ip4_name((struct sockaddr_in*) res_->ai_addr, ip_addr, 16);
        log_info("%s.ip=%s", cli->m_addr, ip_addr);
        strcpy(cli->m_addr, ip_addr);
        int r = uv_ip4_addr(cli->m_addr, cli->m_port, &cli->m_socketaddr);
        if (0 != r) log_err("bad resolving:%s, ", cli->m_addr);
    }

    free(resolver_);
    uv_freeaddrinfo(res_);
}

int br_udp_client_register(mmpool_item_t* cli_pool_item_) {

    br_udp_client_t* cli = (br_udp_client_t*) cli_pool_item_->p;
    int r = uv_udp_init(uv_default_loop(), &cli->m_handler);
    if (0 != r) {
        log_err("new udp client failed :%s", uv_strerror(r));
        return -1;
    }

    /* invalid ip, so we try a DNS resolution */
    if (0 == br_isipv4(cli->m_addr, strlen(cli->m_addr))) {
        uv_getaddrinfo_t* resolver = calloc(1, sizeof (uv_getaddrinfo_t));
        struct addrinfo hints;
        hints.ai_family = PF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        hints.ai_flags = 0;
        resolver->data = cli_pool_item_;

        /* DNS resolution error */
        r = uv_getaddrinfo(uv_default_loop(), resolver,
                on_resolved_udp_client, cli->m_addr, NULL, &hints);

        if (0 != r) {
            log_err("dns resolution failed for:%s, %s", cli->m_addr, uv_strerror(r));
            return -1;
        }

    } else {
        r = uv_ip4_addr(cli->m_addr, cli->m_port, &cli->m_socketaddr);
        if (0 != r) {
            log_err("invalid address:%s, %s", cli->m_addr, uv_strerror(r));
            return -1;
        }
    }
    return 0;
}

int br_udp_client_add(mmpool_t* cli_pool_, const char* addr_, int port_) {

    mmpool_item_t* pool_item = mmpool_take(cli_pool_);
    if (NULL == pool_item) return -1;
    br_udp_client_t* cli = (br_udp_client_t*) pool_item->p;

    strcpy(cli->m_addr, addr_);
    cli->m_port = port_;
    if (0 > br_udp_client_register(pool_item)) return -1;
    return 0;
}

void br_udp_client_send(br_udp_client_t* cli_, const char* str_, size_t len_) {
    uv_buf_t udp_sentence;
    uv_udp_send_t* send_req = (uv_udp_send_t*) calloc(1, sizeof (uv_udp_send_t));
    udp_sentence.base = (char*) strdup(str_);
    udp_sentence.len = len_;
    send_req->data = udp_sentence.base; /* no memory leak */

    int r = uv_udp_send(send_req, &cli_->m_handler, &udp_sentence, 1,
            (const struct sockaddr *) &cli_->m_socketaddr, on_udp_send);

    if (0 != r) {
        log_err("udp send failed for:%s", cli_->m_addr);
    }
}

void br_udp_clients_send(mmpool_t* cli_pool_, const char* str_, size_t len_) {

    mmpool_iter_t iter = {
        .m_index = 0,
        .m_pool = cli_pool_
    };

    br_udp_client_t* pclient = (br_udp_client_t*) mmpool_iter_next(&iter);

    while (pclient) {
        br_udp_client_send(pclient, str_, len_);
        pclient = (br_udp_client_t*) mmpool_iter_next(&iter);
    }
}




/**
 * timestamp reference
 */
static uv_timer_t __brtsref_req;
static unsigned __brtsref = 0;
static char __brtsrefhex[8 + 1] = "00000000";

static void on_tsref_update(uv_timer_t* handle, int status) {
    (
            void) handle;
    (void) status;
    __brtsref = (unsigned) time(NULL);
    snprintf(__brtsrefhex, MM_HEX_TIMESTAMP_LEN + 1, "%x", __brtsref);
}

void br_tsref_init(unsigned refresh_period_) {

    uv_timer_init(uv_default_loop(), &__brtsref_req);
    uv_timer_start(&__brtsref_req, on_tsref_update, 0, refresh_period_);
}

unsigned br_tsref_get() {

    return __brtsref;
}

char* br_tsrefhex_get() {

    return &__brtsrefhex[0];
}

/**
 * common
 */
void br_run(void) {

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}

void br_stop(void) {
    uv_stop(uv_default_loop());
}
