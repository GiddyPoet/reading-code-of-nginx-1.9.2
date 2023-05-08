
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>


static ngx_int_t ngx_http_script_init_arrays(ngx_http_script_compile_t *sc);
static ngx_int_t ngx_http_script_done(ngx_http_script_compile_t *sc);
static ngx_int_t ngx_http_script_add_copy_code(ngx_http_script_compile_t *sc,
    ngx_str_t *value, ngx_uint_t last);
static ngx_int_t ngx_http_script_add_var_code(ngx_http_script_compile_t *sc,
    ngx_str_t *name);
static ngx_int_t ngx_http_script_add_args_code(ngx_http_script_compile_t *sc);
#if (NGX_PCRE)
static ngx_int_t ngx_http_script_add_capture_code(ngx_http_script_compile_t *sc,
     ngx_uint_t n);
#endif
static ngx_int_t
     ngx_http_script_add_full_name_code(ngx_http_script_compile_t *sc);
static size_t ngx_http_script_full_name_len_code(ngx_http_script_engine_t *e);
static void ngx_http_script_full_name_code(ngx_http_script_engine_t *e);


#define ngx_http_script_exit  (u_char *) &ngx_http_script_exit_code

static uintptr_t ngx_http_script_exit_code = (uintptr_t) NULL;


void
ngx_http_script_flush_complex_value(ngx_http_request_t *r,
    ngx_http_complex_value_t *val)
{
    ngx_uint_t *index;

    index = val->flushes;

    if (index) {
        while (*index != (ngx_uint_t) -1) {

            if (r->variables[*index].no_cacheable) {
                r->variables[*index].valid = 0;
                r->variables[*index].not_found = 0;
            }

            index++;
        }
    }
}

//浠