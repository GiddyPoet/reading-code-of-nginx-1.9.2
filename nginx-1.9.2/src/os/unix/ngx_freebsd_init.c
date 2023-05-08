
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>


/* FreeBSD 3.0 at least */
char    ngx_freebsd_kern_ostype[16];
char    ngx_freebsd_kern_osrelease[128];
int     ngx_freebsd_kern_osreldate;
int     ngx_freebsd_hw_ncpu;
int     ngx_freebsd_kern_ipc_somaxconn;
u_long  ngx_freebsd_net_inet_tcp_sendspace;

/* FreeBSD 4.9 */
int     ngx_freebsd_machdep_hlt_logical_cpus;


ngx_uint_t  ngx_freebsd_sendfile_nbytes_bug;
ngx_uint_t  ngx_freebsd_use_tcp_nopush;

ngx_uint_t  ngx_debug_malloc; //if (ngx_debug_malloc)          ngx_memset(p, 0xA5, size)


static ngx_os_io_t ngx_freebsd_io = {
    ngx_unix_recv,
    ngx_readv_chain,
    ngx_udp_unix_recv,
    ngx_unix_send,
#if (NGX_HAVE_SENDFILE)
    ngx_freebsd_sendfile_chain,
    NGX_IO_SENDFILE
#else
    ngx_writev_chain,
    0
#endif
};


typedef struct {
    char        *name;
    void        *value;
    size_t       size;
    ngx_uint_t   exists;
} sysctl_t;


sysctl_t sysctls[] = {
    { "hw.ncpu",
      &ngx_freebsd_hw_ncpu,
      sizeof(ngx_freebsd_hw_ncpu), 0 },

    { "machdep.hlt_logical_cpus",
      &ngx_freebsd_machdep_hlt_logical_cpus,
      sizeof(ngx_freebsd_machdep_hlt_logical_cpus), 0 },

    { "net.inet.tcp.sendspace",
      &ngx_freebsd_net_inet_tcp_sendspace,
      sizeof(ngx_freebsd_net_inet_tcp_sendspace), 0 },

    { "kern.ipc.somaxconn",
      &ngx_freebsd_kern_ipc_somaxconn,
      sizeof(ngx_freebsd_kern_ipc_somaxconn), 0 },

    { NULL, NULL, 0, 0 }
};

void ngx_debug_init(void)
{
#if (NGX_DEBUG_MALLOC)

#if __FreeBSD_version >= 500014 && __FreeBSD_version < 1000011
    _malloc_options = "J";
#elif __FreeBSD_version < 500014
    malloc_options = "J";
#endif

    ngx_debug_malloc = 1;

#else
    char  *mo;

    mo = getenv("MALLOC_OPTIONS"); 

    if (mo && ngx_strchr(mo, 'J')) {//getenv