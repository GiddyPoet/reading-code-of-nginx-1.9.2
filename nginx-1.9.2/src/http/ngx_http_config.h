
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_HTTP_CONFIG_H_INCLUDED_
#define _NGX_HTTP_CONFIG_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

/*
Nginx配置文件详解

#运行用户 
user nobody nobody; 
#启动进程 
worker_processes 2; 
#全局错误日志及PID文件 
error_log logs/error.log notice; 
pid logs/nginx.pid; 
#工作模式及连接数上限 
events { 
use epoll; 
worker_connections 1024; 
} 
#设定http服务器，利用它的反向代理功能提供负载均衡支持 
http { 
#设定mime类型 
include conf/mime.types; 
default_type application/octet-stream; 
#设定日志格式 
log_format main ‘$remote_addr 