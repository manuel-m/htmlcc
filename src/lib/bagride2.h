#ifndef BR_NET_H
#define	BR_NET_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "uv.h"
#include "http_parser.h"
#include "hashmap.h"    

#include "mmpool.h"    

#define br_buf_t uv_buf_t
#define br_tcp_t uv_tcp_t
#define br_udp_t uv_udp_t

#define BR_MAX_CONNECTIONS 64
#define BR_MAX_ADDR_SIZE 2048


    /**
     * tcp
     **/
#define MM_MAX_HOST_NAME 256

typedef struct br_tcp_server_s {
        int m_port;
        struct sockaddr_in m_socketaddr;
        br_tcp_t m_server_handler;
        br_buf_t m_write_buffer;
        void* m_user_parse_cb;
        void* m_data;
        char m_name[MM_MAX_HOST_NAME];
        mmpool_t* m_clients;
    } br_tcp_server_t;

    typedef struct br_tcp_client_s {
        int m_port;
        uv_tcp_t m_handler;
        uv_connect_t m_connect;
        void* m_user_parse_cb;
        struct sockaddr_in m_socketaddr;
        char m_addr[BR_MAX_ADDR_SIZE];
        char m_name[MM_MAX_HOST_NAME];
    } br_tcp_client_t;


    /**
     * parse incomming stream fragment
     * -> set write buffer if write required
     * -> return 0 on success
     */
    typedef int (*br_tcp_server_parser_cb)(ssize_t nread_, const br_buf_t* pbuf_,
            br_tcp_server_t* pserver_);

    typedef int (*br_tcp_client_parser_cb)(ssize_t nread_, const br_buf_t* pbuf_,
            br_tcp_client_t* client_);

    /**
     * udp
     **/
    typedef struct br_udp_server_s {
        br_udp_t m_handler;
        void* m_user_parse_cb;
        void* m_data;
        struct sockaddr_in m_socketaddr;
        int m_port;
    } br_udp_server_t;

    typedef struct br_udp_client_s {
        int m_port;
        br_udp_t m_handler;
        struct sockaddr_in m_socketaddr;
        char m_addr[BR_MAX_ADDR_SIZE];
    } br_udp_client_t;

    typedef int (*br_udp_server_parser_cb)(ssize_t nread_, const br_buf_t* pbuf_,
            br_udp_server_t* pserver_);


    /**
     * @param cli_pool_item
     *   mmpool_item_t instead of handle
     *    to rollback if needed (bad DNS resolution)
     * @return 0 on success
     */
    int br_udp_client_register(mmpool_item_t* cli_pool_item_);

    /**
     * http
     */
    typedef struct br_http_server_s {
        struct sockaddr_in m_addr;
        uv_tcp_t m_handler;
        http_parser_settings m_parser_settings;
        int m_request_num;
        int m_port;
        void* m_gen_response_cb;
        map_t m_resources;
    } br_http_server_t;

#define BR_MAX_REQ_URL_LEN 2048

    typedef struct br_http_client_s {
        uv_tcp_t m_handle;
        http_parser m_parser;
        uv_write_t m_write_req;
        int m_request_num;
        br_http_server_t* m_server;
        uv_buf_t m_resbuf;
        char requested_url[BR_MAX_REQ_URL_LEN];
    } br_http_client_t;

    typedef struct br_http_resource_s {
        size_t m_len;
        const char* m_data;
    } br_http_resource_t;

    int br_out_tcp_write_string(br_tcp_server_t*, const char*, size_t len_);


    typedef int (*br_http_server_parser_cb)(br_http_client_t* cli_);


    int br_udp_client_add(mmpool_t* cli_pool_, const char* addr_, int port_);
    void br_udp_clients_send(mmpool_t* cli_pool_, const char* str_, size_t len_);

    int br_udp_server_init(br_udp_server_t* srv_, int port_, void* user_parse_cb_);

    int br_tcp_server_init(br_tcp_server_t* server_, const char* name_, int port_,
            void* user_parse_cb_, int max_connections_);

    int br_tcp_client_init(br_tcp_client_t* client_, const char* name_,
            const char* addr_, int port_, void* user_parse_cb_);

    void br_tcp_server_close(br_tcp_server_t* srv_);

    int br_http_server_init(br_http_server_t* srv_, int port_, void* gen_response_cb_);
    int br_http_server_resource_add(br_http_server_t* srv_, const char* key,
            const char* data_, size_t size_);


    /**
     * timestamp
     */
    void br_tsref_init();
    unsigned br_tsref_get();
    char* br_tsrefhex_get();
#define MM_HEX_TIMESTAMP_LEN 8

    /**
     * common
     */
    void br_run(void);
    void br_stop(void);

#ifdef	__cplusplus
}
#endif

#endif	/* BR_NET_H */
