
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_event.h>


#define NGX_SSL_PASSWORD_BUFFER_SIZE  4096


typedef struct {
    ngx_uint_t  engine;   /* unsigned  engine:1; */
} ngx_openssl_conf_t;


static int ngx_ssl_password_callback(char *buf, int size, int rwflag,
    void *userdata);
static int ngx_ssl_verify_callback(int ok, X509_STORE_CTX *x509_store);
static void ngx_ssl_info_callback(const ngx_ssl_conn_t *ssl_conn, int where,
    int ret);
static void ngx_ssl_passwords_cleanup(void *data);
static void ngx_ssl_handshake_handler(ngx_event_t *ev);
static ngx_int_t ngx_ssl_handle_recv(ngx_connection_t *c, int n);
static void ngx_ssl_write_handler(ngx_event_t *wev);
static void ngx_ssl_read_handler(ngx_event_t *rev);
static void ngx_ssl_shutdown_handler(ngx_event_t *ev);
static void ngx_ssl_connection_error(ngx_connection_t *c, int sslerr,
    ngx_err_t err, char *text);
static void ngx_ssl_clear_error(ngx_log_t *log);

static ngx_int_t ngx_ssl_session_id_context(ngx_ssl_t *ssl,
    ngx_str_t *sess_ctx);
ngx_int_t ngx_ssl_session_cache_init(ngx_shm_zone_t *shm_zone, void *data);
static int ngx_ssl_new_session(ngx_ssl_conn_t *ssl_conn,
    ngx_ssl_session_t *sess);
static ngx_ssl_session_t *ngx_ssl_get_cached_session(ngx_ssl_conn_t *ssl_conn,
    u_char *id, int len, int *copy);
static void ngx_ssl_remove_session(SSL_CTX *ssl, ngx_ssl_session_t *sess);
static void ngx_ssl_expire_sessions(ngx_ssl_session_cache_t *cache,
    ngx_slab_pool_t *shpool, ngx_uint_t n);
static void ngx_ssl_session_rbtree_insert_value(ngx_rbtree_node_t *temp,
    ngx_rbtree_node_t *node, ngx_rbtree_node_t *sentinel);

#ifdef SSL_CTRL_SET_TLSEXT_TICKET_KEY_CB
static int ngx_ssl_session_ticket_key_callback(ngx_ssl_conn_t *ssl_conn,
    unsigned char *name, unsigned char *iv, EVP_CIPHER_CTX *ectx,
    HMAC_CTX *hctx, int enc);
#endif

#if (OPENSSL_VERSION_NUMBER < 0x10002002L || defined LIBRESSL_VERSION_NUMBER)
static ngx_int_t ngx_ssl_check_name(ngx_str_t *name, ASN1_STRING *str);
#endif

static void *ngx_openssl_create_conf(ngx_cycle_t *cycle);
static char *ngx_openssl_engine(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static void ngx_openssl_exit(ngx_cycle_t *cycle);


static ngx_command_t  ngx_openssl_commands[] = { //