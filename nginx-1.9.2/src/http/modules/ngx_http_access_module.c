
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>


typedef struct {
    in_addr_t         mask;
    in_addr_t         addr;
    ngx_uint_t        deny;      /* unsigned  deny:1; */
} ngx_http_access_rule_t;

#if (NGX_HAVE_INET6)

typedef struct {
    struct in6_addr   addr;
    struct in6_addr   mask;
    ngx_uint_t        deny;      /* unsigned  deny:1; */
} ngx_http_access_rule6_t;

#endif

#if (NGX_HAVE_UNIX_DOMAIN)

typedef struct {
    ngx_uint_t        deny;      /* unsigned  deny:1; */
} ngx_http_access_rule_un_t;

#endif

typedef struct {
    ngx_array_t      *rules;     /* array of ngx_http_access_rule_t */
#if (NGX_HAVE_INET6)
    ngx_array_t      *rules6;    /* array of ngx_http_access_rule6_t */
#endif
#if (NGX_HAVE_UNIX_DOMAIN)
    ngx_array_t      *rules_un;  /* array of ngx_http_access_rule_un_t */
#endif
} ngx_http_access_loc_conf_t;


static ngx_int_t ngx_http_access_handler(ngx_http_request_t *r);
static ngx_int_t ngx_http_access_inet(ngx_http_request_t *r,
    ngx_http_access_loc_conf_t *alcf, in_addr_t addr);
#if (NGX_HAVE_INET6)
static ngx_int_t ngx_http_access_inet6(ngx_http_request_t *r,
    ngx_http_access_loc_conf_t *alcf, u_char *p);
#endif
#if (NGX_HAVE_UNIX_DOMAIN)
static ngx_int_t ngx_http_access_unix(ngx_http_request_t *r,
    ngx_http_access_loc_conf_t *alcf);
#endif
static ngx_int_t ngx_http_access_found(ngx_http_request_t *r, ngx_uint_t deny);
static char *ngx_http_access_rule(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf);
static void *ngx_http_access_create_loc_conf(ngx_conf_t *cf);
static char *ngx_http_access_merge_loc_conf(ngx_conf_t *cf,
    void *parent, void *child);
static ngx_int_t ngx_http_access_init(ngx_conf_t *cf);


/*
HttpAccess妯″