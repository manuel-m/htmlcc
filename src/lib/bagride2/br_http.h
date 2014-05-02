#ifndef BR_HTTP_H
#define	BR_HTTP_H

#include "uv.h"
#include "http_parser.h"
#include "hashmap.h"
#include "rsr.h"

#ifdef	__cplusplus
extern "C" {
#endif

#define BR_MAX_CONNECTIONS 64    
#define BR_MAX_HEADERS 13
#define BR_MAX_ELEMENT_SIZE 2048
    
    typedef struct br_http_srv_s {
        struct sockaddr_in m_addr;
        uv_tcp_t m_handler;
        http_parser_settings m_parser_settings;
        int m_request_num;
        int m_port;
        void* m_gen_response_cb;
        const char* m_rsr_404;
        map_t m_static_resources;
        map_t m_types;
    } br_http_srv_t;



    typedef struct br_http_cli_s {
        uv_tcp_t m_handle;
        http_parser m_parser;
        uv_write_t m_write_req;
        int m_request_num;
        br_http_srv_t* m_srv;
        uv_buf_t m_resbuf;

        struct {
            struct message_s {
                const char *name; // for debugging purposes
                const char *raw;
                enum http_parser_type type;
                enum http_method method;
                int status_code;
                char response_status[BR_MAX_ELEMENT_SIZE];
//                char request_path[BR_MAX_ELEMENT_SIZE];
                char request_url[BR_MAX_ELEMENT_SIZE];
//                char fragment[BR_MAX_ELEMENT_SIZE];
//                char query_string[BR_MAX_ELEMENT_SIZE];
//                char body[BR_MAX_ELEMENT_SIZE];
                size_t body_size;
                const char *host;
                const char *userinfo;
                uint16_t port;
                int num_headers;

                enum {
                    NONE = 0, BR_HEADER_FIELD, BR_HEADER_VALUE
                } last_header_element;
                char headers [BR_MAX_HEADERS][2][BR_MAX_ELEMENT_SIZE];
                int should_keep_alive;

                const char *upgrade; // upgraded body

                unsigned short http_major;
                unsigned short http_minor;

                int message_begin_cb_called;
                int headers_complete_cb_called;
                int message_complete_cb_called;
//                int message_complete_on_eof;
                int body_is_final;
                
            } m_mess;

        } pub;
    } br_http_cli_t;
    
    typedef int (*br_http_srv_parser_cb)(br_http_cli_t* cli_);

    typedef struct br_http_type_item_s {
        const char* id;
        const char* response_type;
    } br_http_type_item_t;
    
    typedef struct br_http_srv_spec_s {
        int m_port;
        const rsrs_t* m_static_resources;
        const char* m_rsr_404;
    } br_http_srv_spec_t;

    int br_http_srv_init(br_http_srv_t* srv_, const br_http_srv_spec_t* spec_);
    int br_http_srv_static_rsr_add(br_http_srv_t* srv_, const rsr_t* rsr_);
   

#ifdef	__cplusplus
}
#endif

#endif	/* BR_HTTP_H */

