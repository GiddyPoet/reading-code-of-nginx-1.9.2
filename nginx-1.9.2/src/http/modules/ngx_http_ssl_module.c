
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>


typedef ngx_int_t (*ngx_ssl_variable_handler_pt)(ngx_connection_t *c,
    ngx_pool_t *pool, ngx_str_t *s);


#define NGX_DEFAULT_CIPHERS     "HIGH:!aNULL:!MD5"
#define NGX_DEFAULT_ECDH_CURVE  "prime256v1"

#define NGX_HTTP_NPN_ADVERTISE  "\x08http/1.1"


#ifdef TLSEXT_TYPE_application_layer_protocol_negotiation
static int ngx_http_ssl_alpn_select(ngx_ssl_conn_t *ssl_conn,
    const unsigned char **out, unsigned char *outlen,
    const unsigned char *in, unsigned int inlen, void *arg);
#endif

#ifdef TLSEXT_TYPE_next_proto_neg
static int ngx_http_ssl_npn_advertised(ngx_ssl_conn_t *ssl_conn,
    const unsigned char **out, unsigned int *outlen, void *arg);
#endif

static ngx_int_t ngx_http_ssl_static_variable(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_ssl_variable(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);

static ngx_int_t ngx_http_ssl_add_variables(ngx_conf_t *cf);
static void *ngx_http_ssl_create_srv_conf(ngx_conf_t *cf);
static char *ngx_http_ssl_merge_srv_conf(ngx_conf_t *cf,
    void *parent, void *child);

static char *ngx_http_ssl_enable(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf);
static char *ngx_http_ssl_password_file(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf);
static char *ngx_http_ssl_session_cache(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf);

static ngx_int_t ngx_http_ssl_init(ngx_conf_t *cf);


static ngx_conf_bitmask_t  ngx_http_ssl_protocols[] = {
    { ngx_string("SSLv2"), NGX_SSL_SSLv2 },
    { ngx_string("SSLv3"), NGX_SSL_SSLv3 },
    { ngx_string("TLSv1"), NGX_SSL_TLSv1 },
    { ngx_string("TLSv1.1"), NGX_SSL_TLSv1_1 },
    { ngx_string("TLSv1.2"), NGX_SSL_TLSv1_2 },
    { ngx_null_string, 0 }
};


static ngx_conf_enum_t  ngx_http_ssl_verify[] = {
    { ngx_string("off"), 0 },
    { ngx_string("on"), 1 },
    { ngx_string("optional"), 2 },
    { ngx_string("optional_no_ca"), 3 },
    { ngx_null_string, 0 }
};


static ngx_command_t  ngx_http_ssl_commands[] = {

    { ngx_string("ssl"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_CONF_FLAG,
      ngx_http_ssl_enable,
      NGX_HTTP_SRV_CONF_OFFSET,
      offsetof(ngx_http_ssl_srv_conf_t, enable),
      NULL },

    { ngx_string("ssl_certificate"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_str_slot,
      NGX_HTTP_SRV_CONF_OFFSET,
      offsetof(ngx_http_ssl_srv_conf_t, certificate),
      NULL },

    { ngx_string("ssl_certificate_key"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_str_slot,
      NGX_HTTP_SRV_CONF_OFFSET,
      offsetof(ngx_http_ssl_srv_conf_t, certificate_key),
      NULL },

    { ngx_string("ssl_password_file"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_CONF_TAKE1,
      ngx_http_ssl_password_file,
      NGX_HTTP_SRV_CONF_OFFSET,
      0,
      NULL },

    { ngx_string("ssl_dhparam"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_str_slot,
      NGX_HTTP_SRV_CONF_OFFSET,
      offsetof(ngx_http_ssl_srv_conf_t, dhparam),
      NULL },

    { ngx_string("ssl_ecdh_curve"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_str_slot,
      NGX_HTTP_SRV_CONF_OFFSET,
      offsetof(ngx_http_ssl_srv_conf_t, ecdh_curve),
      NULL },

    { ngx_string("ssl_protocols"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_CONF_1MORE,
      ngx_conf_set_bitmask_slot,
      NGX_HTTP_SRV_CONF_OFFSET,
      offsetof(ngx_http_ssl_srv_conf_t, protocols),
      &ngx_http_ssl_protocols },

    { ngx_string("ssl_ciphers"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_str_slot,
      NGX_HTTP_SRV_CONF_OFFSET,
      offsetof(ngx_http_ssl_srv_conf_t, ciphers),
      NULL },

    { ngx_string("ssl_buffer_size"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_size_slot,
      NGX_HTTP_SRV_CONF_OFFSET,
      offsetof(ngx_http_ssl_srv_conf_t, buffer_size),
      NULL },

    { ngx_string("ssl_verify_client"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_enum_slot,
      NGX_HTTP_SRV_CONF_OFFSET,
      offsetof(ngx_http_ssl_srv_conf_t, verify),
      &ngx_http_ssl_verify },

    { ngx_string("ssl_verify_depth"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_num_slot,
      NGX_HTTP_SRV_CONF_OFFSET,
      offsetof(ngx_http_ssl_srv_conf_t, verify_depth),
      NULL },

    { ngx_string("ssl_client_certificate"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_str_slot,
      NGX_HTTP_SRV_CONF_OFFSET,
      offsetof(ngx_http_ssl_srv_conf_t, client_certificate),
      NULL },

    { ngx_string("ssl_trusted_certificate"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_str_slot,
      NGX_HTTP_SRV_CONF_OFFSET,
      offsetof(ngx_http_ssl_srv_conf_t, trusted_certificate),
      NULL },

    { ngx_string("ssl_prefer_server_ciphers"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_CONF_FLAG,
      ngx_conf_set_flag_slot,
      NGX_HTTP_SRV_CONF_OFFSET,
      offsetof(ngx_http_ssl_srv_conf_t, prefer_server_ciphers),
      NULL },

    { ngx_string("ssl_session_cache"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_CONF_TAKE12,
      ngx_http_ssl_session_cache,
      NGX_HTTP_SRV_CONF_OFFSET,
      0,
      NULL },

    { ngx_string("ssl_session_tickets"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_CONF_FLAG,
      ngx_conf_set_flag_slot,
      NGX_HTTP_SRV_CONF_OFFSET,
      offsetof(ngx_http_ssl_srv_conf_t, session_tickets),
      NULL },

    { ngx_string("ssl_session_ticket_key"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_str_array_slot,
      NGX_HTTP_SRV_CONF_OFFSET,
      offsetof(ngx_http_ssl_srv_conf_t, session_ticket_keys),
      NULL },

    { ngx_string("ssl_session_timeout"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_sec_slot,
      NGX_HTTP_SRV_CONF_OFFSET,
      offsetof(ngx_http_ssl_srv_conf_t, session_timeout),
      NULL },

    { ngx_string("ssl_crl"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_str_slot,
      NGX_HTTP_SRV_CONF_OFFSET,
      offsetof(ngx_http_ssl_srv_conf_t, crl),
      NULL },

    { ngx_string("ssl_stapling"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_CONF_FLAG,
      ngx_conf_set_flag_slot,
      NGX_HTTP_SRV_CONF_OFFSET,
      offsetof(ngx_http_ssl_srv_conf_t, stapling),
      NULL },

    { ngx_string("ssl_stapling_file"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_str_slot,
      NGX_HTTP_SRV_CONF_OFFSET,
      offsetof(ngx_http_ssl_srv_conf_t, stapling_file),
      NULL },

    { ngx_string("ssl_stapling_responder"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_str_slot,
      NGX_HTTP_SRV_CONF_OFFSET,
      offsetof(ngx_http_ssl_srv_conf_t, stapling_responder),
      NULL },

    { ngx_string("ssl_stapling_verify"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_CONF_FLAG,
      ngx_conf_set_flag_slot,
      NGX_HTTP_SRV_CONF_OFFSET,
      offsetof(ngx_http_ssl_srv_conf_t, stapling_verify),
      NULL },

      ngx_null_command
};


static ngx_http_module_t  ngx_http_ssl_module_ctx = {
    ngx_http_ssl_add_variables,            /* preconfiguration */
    ngx_http_ssl_init,                     /* postconfiguration */

    NULL,                                  /* create main configuration */
    NULL,                                  /* init main configuration */

    ngx_http_ssl_create_srv_conf,          /* create server configuration */
    ngx_http_ssl_merge_srv_conf,           /* merge server configuration */

    NULL,                                  /* create location configuration */
    NULL                                   /* merge location configuration */
};


ngx_module_t  ngx_http_ssl_module = {
    NGX_MODULE_V1,
    &ngx_http_ssl_module_ctx,              /* module context */
    ngx_http_ssl_commands,                 /* module directives */
    NGX_HTTP_MODULE,                       /* module type */
    NULL,                                  /* init master */
    NULL,                                  /* init module */
    NULL,                                  /* init process */
    NULL,                                  /* init thread */
    NULL,                                  /* exit thread */
    NULL,                                  /* exit process */
    NULL,                                  /* exit master */
    NGX_MODULE_V1_PADDING
};


static ngx_http_variable_t  ngx_http_ssl_vars[] = {

    { ngx_string("ssl_protocol"), NULL, ngx_http_ssl_static_variable,
      (uintptr_t) ngx_ssl_get_protocol, NGX_HTTP_VAR_CHANGEABLE, 0 },

    { ngx_string("ssl_cipher"), NULL, ngx_http_ssl_static_variable,
      (uintptr_t) ngx_ssl_get_cipher_name, NGX_HTTP_VAR_CHANGEABLE, 0 },

    { ngx_string("ssl_session_id"), NULL, ngx_http_ssl_variable,
      (uintptr_t) ngx_ssl_get_session_id, NGX_HTTP_VAR_CHANGEABLE, 0 },

    { ngx_string("ssl_session_reused"), NULL, ngx_http_ssl_variable,
      (uintptr_t) ngx_ssl_get_session_reused, NGX_HTTP_VAR_CHANGEABLE, 0 },

    { ngx_string("ssl_server_name"), NULL, ngx_http_ssl_variable,
      (uintptr_t) ngx_ssl_get_server_name, NGX_HTTP_VAR_CHANGEABLE, 0 },

    { ngx_string("ssl_client_cert"), NULL, ngx_http_ssl_variable,
      (uintptr_t) ngx_ssl_get_certificate, NGX_HTTP_VAR_CHANGEABLE, 0 },

    { ngx_string("ssl_client_raw_cert"), NULL, ngx_http_ssl_variable,
      (uintptr_t) ngx_ssl_get_raw_certificate,
      NGX_HTTP_VAR_CHANGEABLE, 0 },

    { ngx_string("ssl_client_s_dn"), NULL, ngx_http_ssl_variable,
      (uintptr_t) ngx_ssl_get_subject_dn, NGX_HTTP_VAR_CHANGEABLE, 0 },

    { ngx_string("ssl_client_i_dn"), NULL, ngx_http_ssl_variable,
      (uintptr_t) ngx_ssl_get_issuer_dn, NGX_HTTP_VAR_CHANGEABLE, 0 },

    { ngx_string("ssl_client_serial"), NULL, ngx_http_ssl_variable,
      (uintptr_t) ngx_ssl_get_serial_number, NGX_HTTP_VAR_CHANGEABLE, 0 },

    { ngx_string("ssl_client_fingerprint"), NULL, ngx_http_ssl_variable,
      (uintptr_t) ngx_ssl_get_fingerprint, NGX_HTTP_VAR_CHANGEABLE, 0 },

    { ngx_string("ssl_client_verify"), NULL, ngx_http_ssl_variable,
      (uintptr_t) ngx_ssl_get_client_verify, NGX_HTTP_VAR_CHANGEABLE, 0 },

    { ngx_null_string, NULL, NULL, 0, 0, 0 }
};


static ngx_str_t ngx_http_ssl_sess_id_ctx = ngx_string("HTTP");


#ifdef TLSEXT_TYPE_application_layer_protocol_negotiation

/*
2017/03/08 15:24:45[                ngx_http_ssl_alpn_select,   341]  [debug] 2794#2794: *2 SSL ALPN supported by client: h2
2017/03/08 15:24:45[                ngx_http_ssl_alpn_select,   341]  [debug] 2794#2794: *2 SSL ALPN supported by client: spdy/3.1
2017/03/08 15:24:45[                ngx_http_ssl_alpn_select,   341]  [debug] 2794#2794: *2 SSL ALPN supported by client: http/1.1
2017/03/08 15:24:45[                ngx_http_ssl_alpn_select,   368]  [debug] 2794#2794: *2 SSL ALPN selected: h2
*/ /* 纭