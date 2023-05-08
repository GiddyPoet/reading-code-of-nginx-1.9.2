/*
root@root:/usr/local/nginx/logs# tail -f error.log 
2016/01/07 12:37:40[        ngx_http_file_cache_manager,  2217]  [debug] 20092#20092: http file cache size: 2, max_size:262144
2016/01/07 12:37:40[                ngx_event_add_timer,   100]  [debug] 20092#20092: <ngx_cache_manager_process_handler,  1648>  event timer add fd:-1, expire-time:10 s, timer.key:442524854
2016/01/07 12:37:50[            ngx_event_expire_timers,   133]  [debug] 20092#20092: event timer del: -1: 442524854
2016/01/07 12:37:50[         ngx_http_file_cache_expire,  2018]  [debug] 20092#20092: http file cache expire
2016/01/07 12:37:50[                     ngx_shmtx_lock,   168]  [debug] 20092#20092: shmtx lock
2016/01/07 12:37:50[                   ngx_shmtx_unlock,   249]  [debug] 20092#20092: shmtx unlock
2016/01/07 12:37:50[                     ngx_shmtx_lock,   168]  [debug] 20092#20092: shmtx lock
2016/01/07 12:37:50[                   ngx_shmtx_unlock,   249]  [debug] 20092#20092: shmtx unlock
2016/01/07 12:37:50[        ngx_http_file_cache_manager,  2217]  [debug] 20092#20092: http file cache size: 2, max_size:262144
2016/01/07 12:37:50[                ngx_event_add_timer,   100]  [debug] 20092#20092: <ngx_cache_manager_process_handler,  1648>  event timer add fd:-1, expire-time:10 s, timer.key:442534855
2016/01/07 12:37:59[           ngx_epoll_process_events,  1720]  [debug] 20090#20090: epoll: fd:6 EPOLLIN  (ev:0001) d:B2593008
2016/01/07 12:37:59[           ngx_epoll_process_events,  1766]  [debug] 20090#20090: post event B0793008
2016/01/07 12:37:59[           ngx_event_process_posted,    65]  [debug] 20090#20090: begin to run befor posted event B0793008
2016/01/07 12:37:59[           ngx_event_process_posted,    67]  [debug] 20090#20090: delete posted event B0793008
2016/01/07 12:37:59[                   ngx_event_accept,    72]  [debug] 20090#20090: accept on 10.2.13.167:80, ready: 1
2016/01/07 12:37:59[                   ngx_event_accept,   370]  [debug] 20090#20090: *14 accept: 10.2.13.1:54174 fd:12
2016/01/07 12:37:59[                ngx_event_add_timer,   100]  [debug] 20090#20090: *14 < ngx_http_init_connection,   393>  event timer add fd:12, expire-time:20 s, timer.key:442553219
2016/01/07 12:37:59[            ngx_reusable_connection,  1177]  [debug] 20090#20090: *14 reusable connection: 1
2016/01/07 12:37:59[              ngx_handle_read_event,   499]  [debug] 20090#20090: *14 < ngx_http_init_connection,   396> epoll NGX_USE_CLEAR_EVENT(et) read add
2016/01/07 12:37:59[                ngx_epoll_add_event,  1414]  [debug] 20090#20090: *14 epoll add read event: fd:12 op:1 ev:80002001
2016/01/07 12:37:59[                   ngx_event_accept,    99]  [debug] 20090#20090: accept() not ready (11: Resource temporarily unavailable)
2016/01/07 12:37:59[           ngx_trylock_accept_mutex,   405]  [debug] 20090#20090: accept mutex locked
2016/01/07 12:37:59[           ngx_epoll_process_events,  1720]  [debug] 20090#20090: epoll: fd:12 EPOLLIN  (ev:0001) d:B2593158
2016/01/07 12:37:59[           ngx_epoll_process_events,  1766]  [debug] 20090#20090: *14 post event B0793098
2016/01/07 12:37:59[           ngx_event_process_posted,    65]  [debug] 20090#20090: begin to run befor posted event B0793098
2016/01/07 12:37:59[           ngx_event_process_posted,    67]  [debug] 20090#20090: *14 delete posted event B0793098
2016/01/07 12:37:59[      ngx_http_wait_request_handler,   417]  [debug] 20090#20090: *14 http wait request handler
2016/01/07 12:37:59[                      ngx_unix_recv,   204]  [debug] 20090#20090: *14 recv: fd:12 read-size:264 of 1024, ready:1
2016/01/07 12:37:59[            ngx_reusable_connection,  1177]  [debug] 20090#20090: *14 reusable connection: 0
2016/01/07 12:37:59[      ngx_http_process_request_line,   963]  [debug] 20090#20090: *14 http process request line
2016/01/07 12:37:59[      ngx_http_process_request_line,  1002]  [debug] 20090#20090: *14 http request line: "GET /xiazai.php HTTP/1.1"
2016/01/07 12:37:59[       ngx_http_process_request_uri,  1223]  [debug] 20090#20090: *14 http uri: "/xiazai.php"
2016/01/07 12:37:59[       ngx_http_process_request_uri,  1226]  [debug] 20090#20090: *14 http args: ""
2016/01/07 12:37:59[       ngx_http_process_request_uri,  1229]  [debug] 20090#20090: *14 http exten: "php"
2016/01/07 12:37:59[   ngx_http_process_request_headers,  1272]  [debug] 20090#20090: *14 http process request header line
2016/01/07 12:37:59[   ngx_http_process_request_headers,  1404]  [debug] 20090#20090: *14 http header: "Accept: text/html, application/xhtml+xml, * / *"
2016/01/07 12:37:59[   ngx_http_process_request_headers,  1404]  [debug] 20090#20090: *14 http header: "Accept-Language: zh-CN"
2016/01/07 12:37:59[   ngx_http_process_request_headers,  1404]  [debug] 20090#20090: *14 http header: "User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64; Trident/7.0; rv:11.0) like Gecko"
2016/01/07 12:37:59[   ngx_http_process_request_headers,  1404]  [debug] 20090#20090: *14 http header: "Accept-Encoding: gzip, deflate"
2016/01/07 12:37:59[   ngx_http_process_request_headers,  1404]  [debug] 20090#20090: *14 http header: "Host: 10.2.13.167"
2016/01/07 12:37:59[   ngx_http_process_request_headers,  1404]  [debug] 20090#20090: *14 http header: "DNT: 1"
2016/01/07 12:37:59[   ngx_http_process_request_headers,  1404]  [debug] 20090#20090: *14 http header: "Connection: Keep-Alive"
2016/01/07 12:37:59[   ngx_http_process_request_headers,  1414]  [debug] 20090#20090: *14 http header done
2016/01/07 12:37:59[                ngx_event_del_timer,    39]  [debug] 20090#20090: *14 < ngx_http_process_request,  2015>  event timer del: 12: 442553219
2016/01/07 12:37:59[        ngx_http_core_rewrite_phase,  1962]  [debug] 20090#20090: *14 rewrite phase: 0 (NGX_HTTP_SERVER_REWRITE_PHASE)
2016/01/07 12:37:59[    ngx_http_core_find_config_phase,  2020]  [debug] 20090#20090: *14 find config phase: 1 (NGX_HTTP_FIND_CONFIG_PHASE), uri:/xiazai.php
2016/01/07 12:37:59[ ngx_http_core_find_static_location,  2907]  [debug] 20090#20090: *14 static_locations test location: "/", client uri:/xiazai.php
2016/01/07 12:37:59[ ngx_http_core_find_static_location,  2907]  [debug] 20090#20090: *14 static_locations test location: "mytest_upstream", client uri:/xiazai.php
2016/01/07 12:37:59[ ngx_http_core_find_static_location,  2907]  [debug] 20090#20090: *14 static_locations test location: "proxy1/", client uri:/xiazai.php
2016/01/07 12:37:59[        ngx_http_core_find_location,  2847]  [debug] 20090#20090: *14 ngx pcre test location: ~ "/download/"
2016/01/07 12:37:59[        ngx_http_core_find_location,  2847]  [debug] 20090#20090: *14 ngx pcre test location: ~ "\.php$"
2016/01/07 12:37:59[    ngx_http_core_find_config_phase,  2040]  [debug] 20090#20090: *14 using configuration "\.php$"
2016/01/07 12:37:59[    ngx_http_core_find_config_phase,  2047]  [debug] 20090#20090: *14 http cl:-1 max:1048576, rc:0
2016/01/07 12:37:59[        ngx_http_core_rewrite_phase,  1962]  [debug] 20090#20090: *14 rewrite phase: 2 (NGX_HTTP_REWRITE_PHASE)
2016/01/07 12:37:59[   ngx_http_core_post_rewrite_phase,  2115]  [debug] 20090#20090: *14 post rewrite phase: 3 (NGX_HTTP_POST_REWRITE_PHASE)
2016/01/07 12:37:59[        ngx_http_core_generic_phase,  1898]  [debug] 20090#20090: *14 generic phase: 4 (NGX_HTTP_PREACCESS_PHASE)
2016/01/07 12:37:59[        ngx_http_core_generic_phase,  1898]  [debug] 20090#20090: *14 generic phase: 5 (NGX_HTTP_PREACCESS_PHASE)
2016/01/07 12:37:59[         ngx_http_core_access_phase,  2213]  [debug] 20090#20090: *14 access phase: 6 (NGX_HTTP_ACCESS_PHASE)
2016/01/07 12:37:59[         ngx_http_core_access_phase,  2213]  [debug] 20090#20090: *14 access phase: 7 (NGX_HTTP_ACCESS_PHASE)
2016/01/07 12:37:59[    ngx_http_core_post_access_phase,  2315]  [debug] 20090#20090: *14 post access phase: 8 (NGX_HTTP_POST_ACCESS_PHASE)
2016/01/07 12:37:59[        ngx_http_core_content_phase,  2637]  [debug] 20090#20090: *14 content phase(content_handler): 9 (NGX_HTTP_CONTENT_PHASE)
2016/01/07 12:37:59[             ngx_http_upstream_init,   632]  [debug] 20090#20090: *14 http init upstream, client timer: 0
2016/01/07 12:37:59[             ngx_http_upstream_init,   666]  [debug] 20090#20090: *14 <   ngx_http_upstream_init,   665> epoll NGX_WRITE_EVENT(et) write add
2016/01/07 12:37:59[                ngx_epoll_add_event,  1410]  [debug] 20090#20090: *14 epoll modify read and write event: fd:12 op:3 ev:80002005
2016/01/07 12:37:59[        ngx_http_upstream_cache_get,  1156][yangya  [debug] 20090#20090: *14 ngx http upstream cache get use keys_zone:fcgi
2016/01/07 12:37:59[      ngx_http_script_copy_var_code,   989]  [debug] 20090#20090: *14 http script var: "/xiazai.php"
2016/01/07 12:37:59[     ngx_http_file_cache_create_key,   253]  [debug] 20090#20090: *14 http cache key: "/xiazai.php"
2016/01/07 12:37:59[                     ngx_shmtx_lock,   168]  [debug] 20090#20090: shmtx lock
2016/01/07 12:37:59[                   ngx_shmtx_unlock,   249]  [debug] 20090#20090: shmtx unlock
2016/01/07 12:37:59[           ngx_http_file_cache_open,   325]  [debug] 20090#20090: *14 http file cache exists, rc: 0 if exist:1
2016/01/07 12:37:59[           ngx_http_file_cache_name,  1107]  [debug] 20090#20090: *14 cache file: "/var/yyz/cache_xxx/0/e2/7e8e60341921f47ebd97367bfc623e20"
2016/01/07 12:37:59[           ngx_http_file_cache_open,   406]  [debug] 20090#20090: *14 http file cache fd: 13
2016/01/07 12:37:59[                    ngx_thread_read,   147]  [debug] 20090#20090: *14 thread read: fd:13, buf:080F3868, size:4096, offset:0
2016/01/07 12:37:59[              ngx_thread_mutex_lock,   145]  [debug] 20090#20090: pthread_mutex_lock(080F03B0) enter
2016/01/07 12:37:59[               ngx_thread_task_post,   280][yangya  [debug] 20090#20090: ngx add task to thread, task id:181
2016/01/07 12:37:59[             ngx_thread_cond_signal,    54]  [debug] 20090#20090: pthread_cond_signal(080F03D4)
2016/01/07 12:37:59[            ngx_thread_mutex_unlock,   171]  [debug] 20090#20090: pthread_mutex_unlock(080F03B0) exit
2016/01/07 12:37:59[               ngx_thread_task_post,   297]  [debug] 20090#20090: task #181 added to thread pool name: "yang_pool" complete
2016/01/07 12:37:59[            ngx_http_upstream_cache,  1060]  [debug] 20090#20090: *14 http upstream cache: -2
2016/01/07 12:37:59[     ngx_http_upstream_init_request,   710][yangya  [debug] 20090#20090: *14 ngx http cache, conf->cache:1, rc:-3
2016/01/07 12:37:59[          ngx_http_finalize_request,  2592]  [debug] 20090#20090: *14 http finalize request rc: -4, "/xiazai.php?" a:1, c:2
2016/01/07 12:37:59[             ngx_http_close_request,  3921]  [debug] 20090#20090: *14 http request count:2 blk:1
2016/01/07 12:37:59[           ngx_trylock_accept_mutex,   405]  [debug] 20090#20090: accept mutex locked
2016/01/07 12:37:59[           ngx_epoll_process_events,  1720]  [debug] 20090#20090: epoll: fd:12 EPOLLOUT  (ev:0004) d:B2593158
2016/01/07 12:37:59[           ngx_epoll_process_events,  1793]  [debug] 20090#20090: *14 post event AE993098
2016/01/07 12:37:59[           ngx_event_process_posted,    65]  [debug] 20090#20090: begin to run befor posted event AE993098
2016/01/07 12:37:59[           ngx_event_process_posted,    67]  [debug] 20090#20090: *14 delete posted event AE993098
2016/01/07 12:37:59[           ngx_http_request_handler,  2401]  [debug] 20090#20090: *14 http run request(ev->write:1): "/xiazai.php?"
2016/01/07 12:37:59[           ngx_trylock_accept_mutex,   405]  [debug] 20090#20090: accept mutex locked
2016/01/07 12:37:59[               ngx_thread_cond_wait,    96]  [debug] 20090#20094: pthread_cond_wait(080F03D4) exit
2016/01/07 12:37:59[            ngx_thread_mutex_unlock,   171]  [debug] 20090#20094: pthread_mutex_unlock(080F03B0) exit
2016/01/07 12:37:59[              ngx_thread_pool_cycle,   370]  [debug] 20090#20094: run task #181 in thread pool name:"yang_pool"
2016/01/07 12:37:59[            ngx_thread_read_handler,   201]  [debug] 20090#20094: thread read handler
2016/01/07 12:37:59[            ngx_thread_read_handler,   219]  [debug] 20090#20094: pread: 399 (err: 0) of 4096 @0
2016/01/07 12:37:59[              ngx_thread_pool_cycle,   376]  [debug] 20090#20094: complete task #181 in thread pool name: "yang_pool"
2016/01/07 12:37:59[              ngx_thread_mutex_lock,   145]  [debug] 20090#20094: pthread_mutex_lock(080F03B0) enter
2016/01/07 12:37:59[               ngx_thread_cond_wait,    70]  [debug] 20090#20094: pthread_cond_wait(080F03D4) enter
2016/01/07 12:37:59[           ngx_epoll_process_events,  1720]  [debug] 20090#20090: epoll: fd:9 EPOLLIN  (ev:0001) d:080E36C0
2016/01/07 12:37:59[           ngx_epoll_process_events,  1766]  [debug] 20090#20090: post event 080E3680
2016/01/07 12:37:59[           ngx_event_process_posted,    65]  [debug] 20090#20090: begin to run befor posted event 080E3680
2016/01/07 12:37:59[           ngx_event_process_posted,    67]  [debug] 20090#20090: delete posted event 080E3680
2016/01/07 12:37:59[            ngx_thread_pool_handler,   401]  [debug] 20090#20090: thread pool handler
2016/01/07 12:37:59[            ngx_thread_pool_handler,   422]  [debug] 20090#20090: run completion handler for task #181
2016/01/07 12:37:59[ngx_http_cache_thread_event_handler,   939]  [debug] 20090#20090: *14 http file cache thread: "/xiazai.php?"
2016/01/07 12:37:59[                    ngx_thread_read,   147]  [debug] 20090#20090: *14 thread read: fd:13, buf:080F3868, size:4096, offset:0
2016/01/07 12:37:59[            ngx_http_upstream_cache,  1060]  [debug] 20090#20090: *14 http upstream cache: 0
2016/01/07 12:37:59[    ngx_http_fastcgi_process_record,  3109]  [debug] 20090#20090: *14 http fastcgi record byte: 01
2016/01/07 12:37:59[    ngx_http_fastcgi_process_record,  3109]  [debug] 20090#20090: *14 http fastcgi record byte: 06
2016/01/07 12:37:59[    ngx_http_fastcgi_process_record,  3109]  [debug] 20090#20090: *14 http fastcgi record byte: 00
2016/01/07 12:37:59[    ngx_http_fastcgi_process_record,  3109]  [debug] 20090#20090: *14 http fastcgi record byte: 01
2016/01/07 12:37:59[    ngx_http_fastcgi_process_record,  3109]  [debug] 20090#20090: *14 http fastcgi record byte: 00
2016/01/07 12:37:59[    ngx_http_fastcgi_process_record,  3109]  [debug] 20090#20090: *14 http fastcgi record byte: F5
2016/01/07 12:37:59[    ngx_http_fastcgi_process_record,  3109]  [debug] 20090#20090: *14 http fastcgi record byte: 03
2016/01/07 12:37:59[    ngx_http_fastcgi_process_record,  3109]  [debug] 20090#20090: *14 http fastcgi record byte: 00
2016/01/07 12:37:59[    ngx_http_fastcgi_process_record,  3181]  [debug] 20090#20090: *14 http fastcgi record length: 245
2016/01/07 12:37:59[    ngx_http_fastcgi_process_header,  2349]  [debug] 20090#20090: *14 http fastcgi parser: 0
2016/01/07 12:37:59[    ngx_http_fastcgi_process_header,  2457]  [debug] 20090#20090: *14 http fastcgi header: "X-Powered-By: PHP/5.2.13"
2016/01/07 12:37:59[    ngx_http_fastcgi_process_header,  2349]  [debug] 20090#20090: *14 http fastcgi parser: 0
2016/01/07 12:37:59[    ngx_http_fastcgi_process_header,  2457]  [debug] 20090#20090: *14 http fastcgi header: "Content-type: text/html"
2016/01/07 12:37:59[    ngx_http_fastcgi_process_header,  2349]  [debug] 20090#20090: *14 http fastcgi parser: 1
2016/01/07 12:37:59[    ngx_http_fastcgi_process_header,  2473]  [debug] 20090#20090: *14 http fastcgi header done
2016/01/07 12:37:59[                ngx_http_cache_send,  1776]  [debug] 20090#20090: *14 http file cache send: /var/yyz/cache_xxx/0/e2/7e8e60341921f47ebd97367bfc623e20
2016/01/07 12:37:59[               ngx_http_send_header,  3319][yangya  [debug] 20090#20090: *14 ngx http send header
2016/01/07 12:37:59[             ngx_http_header_filter,   677]  [debug] 20090#20090: *14 HTTP/1.1 200 OK
Server: nginx/1.9.2
Date: Thu, 07 Jan 2016 04:37:59 GMT
Content-Type: text/html
Transfer-Encoding: chunked
Connection: keep-alive
X-Powered-By: PHP/5.2.13

2016/01/07 12:37:59[              ngx_http_write_filter,   208]  [debug] 20090#20090: *14 write new buf temporary:1 buf-in-file:0, buf->start:080F48B4, buf->pos:080F48B4, buf_size: 180 file_pos: 0, in_file_size: 0
2016/01/07 12:37:59[              ngx_http_write_filter,   248]  [debug] 20090#20090: *14 http write filter: last:0 flush:0 size:180
2016/01/07 12:37:59[              ngx_http_write_filter,   290][yangya  [debug] 20090#20090: *14 send size:180 < min postpone_output:1460, do not send
2016/01/07 12:37:59[             ngx_http_output_filter,  3372]  [debug] 20090#20090: *14 http output filter "/xiazai.php?"
2016/01/07 12:37:59[               ngx_http_copy_filter,   206]  [debug] 20090#20090: *14 http copy filter: "/xiazai.php?", r->aio:0
2016/01/07 12:37:59[                   ngx_output_chain,    67][yangya  [debug] 20090#20090: *14 ctx->sendfile:0, ctx->aio:0, ctx->directio:0
2016/01/07 12:37:59[             ngx_output_chain_as_is,   309][yangya  [debug] 20090#20090: ngx_output_chain_as_is--- buf_special:0, in_file:1, directio:0, buf_in_mem:0,need_in_memory:0, need_in_temp:0, memory:0, mmap:0
2016/01/07 12:37:59[             ngx_output_chain_as_is,   309][yangya  [debug] 20090#20090: ngx_output_chain_as_is--- buf_special:0, in_file:1, directio:0, buf_in_mem:0,need_in_memory:0, need_in_temp:0, memory:0, mmap:0
2016/01/07 12:37:59[                    ngx_thread_read,   147]  [debug] 20090#20090: *14 thread read: fd:13, buf:080F49E8, size:192, offset:207
2016/01/07 12:37:59[              ngx_thread_mutex_lock,   145]  [debug] 20090#20090: pthread_mutex_lock(080F03B0) enter
2016/01/07 12:37:59[               ngx_thread_task_post,   280][yangya  [debug] 20090#20090: ngx add task to thread, task id:182
2016/01/07 12:37:59[             ngx_thread_cond_signal,    54]  [debug] 20090#20090: pthread_cond_signal(080F03D4)
2016/01/07 12:37:59[            ngx_thread_mutex_unlock,   171]  [debug] 20090#20090: pthread_mutex_unlock(080F03B0) exit
2016/01/07 12:37:59[               ngx_thread_task_post,   297]  [debug] 20090#20090: task #182 added to thread pool name: "yang_pool" complete
2016/01/07 12:37:59[               ngx_http_copy_filter,   284]  [debug] 20090#20090: *14 http copy filter rc: -2, buffered:4 "/xiazai.php?"
2016/01/07 12:37:59[     ngx_http_upstream_init_request,   710][yangya  [debug] 20090#20090: *14 ngx http cache, conf->cache:1, rc:-2
2016/01/07 12:37:59[          ngx_http_finalize_request,  2592]  [debug] 20090#20090: *14 http finalize request rc: -2, "/xiazai.php?" a:1, c:1
2016/01/07 12:37:59[                ngx_event_add_timer,   100]  [debug] 20090#20090: *14 <ngx_http_set_write_handler,  3023>  event timer add fd:12, expire-time:60 s, timer.key:442593227
2016/01/07 12:37:59[           ngx_trylock_accept_mutex,   405]  [debug] 20090#20090: accept mutex locked
2016/01/07 12:37:59[               ngx_thread_cond_wait,    96]  [debug] 20090#20094: pthread_cond_wait(080F03D4) exit
2016/01/07 12:37:59[            ngx_thread_mutex_unlock,   171]  [debug] 20090#20094: pthread_mutex_unlock(080F03B0) exit
2016/01/07 12:37:59[              ngx_thread_pool_cycle,   370]  [debug] 20090#20094: run task #182 in thread pool name:"yang_pool"
2016/01/07 12:37:59[            ngx_thread_read_handler,   201]  [debug] 20090#20094: thread read handler
2016/01/07 12:37:59[            ngx_thread_read_handler,   219]  [debug] 20090#20094: pread: 192 (err: 0) of 192 @207
2016/01/07 12:37:59[              ngx_thread_pool_cycle,   376]  [debug] 20090#20094: complete task #182 in thread pool name: "yang_pool"
2016/01/07 12:37:59[              ngx_thread_mutex_lock,   145]  [debug] 20090#20094: pthread_mutex_lock(080F03B0) enter
2016/01/07 12:37:59[               ngx_thread_cond_wait,    70]  [debug] 20090#20094: pthread_cond_wait(080F03D4) enter
2016/01/07 12:37:59[           ngx_epoll_process_events,  1720]  [debug] 20090#20090: epoll: fd:9 EPOLLIN  (ev:0001) d:080E36C0
2016/01/07 12:37:59[           ngx_epoll_process_events,  1766]  [debug] 20090#20090: post event 080E3680
2016/01/07 12:37:59[           ngx_event_process_posted,    65]  [debug] 20090#20090: begin to run befor posted event 080E3680
2016/01/07 12:37:59[           ngx_event_process_posted,    67]  [debug] 20090#20090: delete posted event 080E3680
2016/01/07 12:37:59[            ngx_thread_pool_handler,   401]  [debug] 20090#20090: thread pool handler
2016/01/07 12:37:59[            ngx_thread_pool_handler,   422]  [debug] 20090#20090: run completion handler for task #182
2016/01/07 12:37:59[           ngx_http_request_handler,  2401]  [debug] 20090#20090: *14 http run request(ev->write:1): "/xiazai.php?"
2016/01/07 12:37:59[                    ngx_http_writer,  3052]  [debug] 20090#20090: *14 http writer handler: "/xiazai.php?"
2016/01/07 12:37:59[             ngx_http_output_filter,  3372]  [debug] 20090#20090: *14 http output filter "/xiazai.php?"
2016/01/07 12:37:59[               ngx_http_copy_filter,   206]  [debug] 20090#20090: *14 http copy filter: "/xiazai.php?", r->aio:0
2016/01/07 12:37:59[                   ngx_output_chain,    67][yangya  [debug] 20090#20090: *14 ctx->sendfile:0, ctx->aio:0, ctx->directio:0
2016/01/07 12:37:59[             ngx_output_chain_as_is,   309][yangya  [debug] 20090#20090: ngx_output_chain_as_is--- buf_special:0, in_file:1, directio:0, buf_in_mem:0,need_in_memory:0, need_in_temp:0, memory:0, mmap:0
2016/01/07 12:37:59[                    ngx_thread_read,   147]  [debug] 20090#20090: *14 thread read: fd:13, buf:080F49E8, size:192, offset:207
2016/01/07 12:37:59[           ngx_http_postpone_filter,   176]  [debug] 20090#20090: *14 http postpone filter "/xiazai.php?" 080F4B04
2016/01/07 12:37:59[       ngx_http_chunked_body_filter,   212]  [debug] 20090#20090: *14 http chunk: 192
2016/01/07 12:37:59[       ngx_http_chunked_body_filter,   273]  [debug] 20090#20090: *14 yang test ..........xxxxxxxx ################## lstbuf:1
2016/01/07 12:37:59[              ngx_http_write_filter,   151]  [debug] 20090#20090: *14 write old buf t:1 f:0 080F48B4, pos 080F48B4, size: 180 file: 0, size: 0
2016/01/07 12:37:59[              ngx_http_write_filter,   208]  [debug] 20090#20090: *14 write new buf temporary:1 buf-in-file:0, buf->start:080F4B50, buf->pos:080F4B50, buf_size: 4 file_pos: 0, in_file_size: 0
2016/01/07 12:37:59[              ngx_http_write_filter,   208]  [debug] 20090#20090: *14 write new buf temporary:1 buf-in-file:0, buf->start:080F49E8, buf->pos:080F49E8, buf_size: 192 file_pos: 0, in_file_size: 0
2016/01/07 12:37:59[              ngx_http_write_filter,   208]  [debug] 20090#20090: *14 write new buf temporary:0 buf-in-file:0, buf->start:00000000, buf->pos:080CEF58, buf_size: 7 file_pos: 0, in_file_size: 0
2016/01/07 12:37:59[              ngx_http_write_filter,   248]  [debug] 20090#20090: *14 http write filter: last:1 flush:0 size:383
2016/01/07 12:37:59[              ngx_http_write_filter,   380]  [debug] 20090#20090: *14 http write filter limit 0
2016/01/07 12:37:59[           ngx_linux_sendfile_chain,   201][yangya  [debug] 20090#20090: *14 @@@@@@@@@@@@@@@@@@@@@@@begin ngx_linux_sendfile_chain @@@@@@@@@@@@@@@@@@@
2016/01/07 12:37:59[                         ngx_writev,   238]  [debug] 20090#20090: *14 writev: 383 of 383
2016/01/07 12:37:59[              ngx_http_write_filter,   386]  [debug] 20090#20090: *14 http write filter 00000000
2016/01/07 12:37:59[               ngx_http_copy_filter,   284]  [debug] 20090#20090: *14 http copy filter rc: 0, buffered:0 "/xiazai.php?"
2016/01/07 12:37:59[                    ngx_http_writer,  3118]  [debug] 20090#20090: *14 http writer output filter: 0, "/xiazai.php?"
2016/01/07 12:37:59[                    ngx_http_writer,  3150]  [debug] 20090#20090: *14 http writer done: "/xiazai.php?"
2016/01/07 12:37:59[          ngx_http_finalize_request,  2592]  [debug] 20090#20090: *14 http finalize request rc: 0, "/xiazai.php?" a:1, c:1
2016/01/07 12:37:59[                ngx_event_del_timer,    39]  [debug] 20090#20090: *14 <ngx_http_finalize_request,  2831>  event timer del: 12: 442593227
2016/01/07 12:37:59[             ngx_http_set_keepalive,  3325]  [debug] 20090#20090: *14 set http keepalive handler
2016/01/07 12:37:59[              ngx_http_free_request,  3977]  [debug] 20090#20090: *14 http close request
2016/01/07 12:37:59[               ngx_http_log_handler,   267]  [debug] 20090#20090: *14 http log handler
2016/01/07 12:37:59[        ngx_http_file_cache_cleanup,  1893]  [debug] 20090#20090: *14 http file cache cleanup
2016/01/07 12:37:59[           ngx_http_file_cache_free,  1832]  [debug] 20090#20090: *14 http file cache free, fd: 13
2016/01/07 12:37:59[                     ngx_shmtx_lock,   168]  [debug] 20090#20090: shmtx lock
2016/01/07 12:37:59[                   ngx_shmtx_unlock,   249]  [debug] 20090#20090: shmtx unlock
2016/01/07 12:37:59[             ngx_http_set_keepalive,  3444]  [debug] 20090#20090: *14 hc free: 00000000 0
2016/01/07 12:37:59[             ngx_http_set_keepalive,  3456]  [debug] 20090#20090: *14 hc busy: 00000000 0
2016/01/07 12:37:59[             ngx_http_set_keepalive,  3502]  [debug] 20090#20090: *14 tcp_nodelay
2016/01/07 12:37:59[            ngx_reusable_connection,  1177]  [debug] 20090#20090: *14 reusable connection: 1
2016/01/07 12:37:59[                ngx_event_add_timer,   100]  [debug] 20090#20090: *14 <   ngx_http_set_keepalive,  3532>  event timer add fd:12, expire-time:75 s, timer.key:442608227
2016/01/07 12:37:59[             ngx_http_set_keepalive,  3535]  [debug] 20090#20090: *14 post event B0793098
2016/01/07 12:37:59[           ngx_event_process_posted,    65]  [debug] 20090#20090: begin to run befor posted event B0793098
2016/01/07 12:37:59[           ngx_event_process_posted,    67]  [debug] 20090#20090: *14 delete posted event B0793098
2016/01/07 12:37:59[         ngx_http_keepalive_handler,  3555]  [debug] 20090#20090: *14 http keepalive handler
2016/01/07 12:37:59[                      ngx_unix_recv,   185]  [debug] 20090#20090: *14 recv() not ready (11: Resource temporarily unavailable)
2016/01/07 12:37:59[                      ngx_unix_recv,   204]  [debug] 20090#20090: *14 recv: fd:12 read-size:-2 of 1024, ready:0
2016/01/07 12:37:59[           ngx_trylock_accept_mutex,   405]  [debug] 20090#20090: accept mutex locked
2016/01/07 12:38:00[            ngx_event_expire_timers,   133]  [debug] 20092#20092: event timer del: -1: 442534855
2016/01/07 12:38:00[         ngx_http_file_cache_expire,  2018]  [debug] 20092#20092: http file cache expire
2016/01/07 12:38:00[                     ngx_shmtx_lock,   168]  [debug] 20092#20092: shmtx lock
2016/01/07 12:38:00[                   ngx_shmtx_unlock,   249]  [debug] 20092#20092: shmtx unlock
2016/01/07 12:38:00[                     ngx_shmtx_lock,   168]  [debug] 20092#20092: shmtx lock
2016/01/07 12:38:00[                   ngx_shmtx_unlock,   249]  [debug] 20092#20092: shmtx unlock
2016/01/07 12:38:00[        ngx_http_file_cache_manager,  2217]  [debug] 20092#20092: http file cache size: 2, max_size:262144
2016/01/07 12:38:00[                ngx_event_add_timer,   100]  [debug] 20092#20092: <ngx_cache_manager_process_handler,  1648>  event timer add fd:-1, expire-time:10 s, timer.key:442544857
2016/01/07 12:38:01[           ngx_epoll_process_events,  1720]  [debug] 20090#20090: epoll: fd:12 EPOLLIN EPOLLOUT  (ev:0005) d:B2593158
2016/01/07 12:38:01[           ngx_epoll_process_events,  1766]  [debug] 20090#20090: *14 post event B0793098
2016/01/07 12:38:01[           ngx_epoll_process_events,  1793]  [debug] 20090#20090: *14 post event AE993098
2016/01/07 12:38:01[           ngx_event_process_posted,    65]  [debug] 20090#20090: begin to run befor posted event B0793098
2016/01/07 12:38:01[           ngx_event_process_posted,    67]  [debug] 20090#20090: *14 delete posted event B0793098
2016/01/07 12:38:01[         ngx_http_keepalive_handler,  3555]  [debug] 20090#20090: *14 http keepalive handler
2016/01/07 12:38:01[                      ngx_unix_recv,   204]  [debug] 20090#20090: *14 recv: fd:12 read-size:357 of 1024, ready:1
2016/01/07 12:38:01[            ngx_reusable_connection,  1177]  [debug] 20090#20090: *14 reusable connection: 0
2016/01/07 12:38:01[                ngx_event_del_timer,    39]  [debug] 20090#20090: *14 <ngx_http_keepalive_handler,  3669>  event timer del: 12: 442608227
2016/01/07 12:38:01[      ngx_http_process_request_line,   963]  [debug] 20090#20090: *14 http process request line
2016/01/07 12:38:01[      ngx_http_process_request_line,  1002]  [debug] 20090#20090: *14 http request line: "GET /download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931 HTTP/1.1"
2016/01/07 12:38:01[       ngx_http_process_request_uri,  1223]  [debug] 20090#20090: *14 http uri: "/download/nginx-1.9.2.rar"
2016/01/07 12:38:01[       ngx_http_process_request_uri,  1226]  [debug] 20090#20090: *14 http args: "st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[       ngx_http_process_request_uri,  1229]  [debug] 20090#20090: *14 http exten: "rar"
2016/01/07 12:38:01[   ngx_http_process_request_headers,  1272]  [debug] 20090#20090: *14 http process request header line
2016/01/07 12:38:01[   ngx_http_process_request_headers,  1404]  [debug] 20090#20090: *14 http header: "Accept: text/html, application/xhtml+xml, * / *"
2016/01/07 12:38:01[   ngx_http_process_request_headers,  1404]  [debug] 20090#20090: *14 http header: "Referer: http://10.2.13.167/xiazai.php"
2016/01/07 12:38:01[   ngx_http_process_request_headers,  1404]  [debug] 20090#20090: *14 http header: "Accept-Language: zh-CN"
2016/01/07 12:38:01[   ngx_http_process_request_headers,  1404]  [debug] 20090#20090: *14 http header: "User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64; Trident/7.0; rv:11.0) like Gecko"
2016/01/07 12:38:01[   ngx_http_process_request_headers,  1404]  [debug] 20090#20090: *14 http header: "Accept-Encoding: gzip, deflate"
2016/01/07 12:38:01[   ngx_http_process_request_headers,  1404]  [debug] 20090#20090: *14 http header: "Host: 10.2.13.167"
2016/01/07 12:38:01[   ngx_http_process_request_headers,  1404]  [debug] 20090#20090: *14 http header: "DNT: 1"
2016/01/07 12:38:01[   ngx_http_process_request_headers,  1404]  [debug] 20090#20090: *14 http header: "Connection: Keep-Alive"
2016/01/07 12:38:01[   ngx_http_process_request_headers,  1414]  [debug] 20090#20090: *14 http header done
2016/01/07 12:38:01[        ngx_http_core_rewrite_phase,  1962]  [debug] 20090#20090: *14 rewrite phase: 0 (NGX_HTTP_SERVER_REWRITE_PHASE)
2016/01/07 12:38:01[    ngx_http_core_find_config_phase,  2020]  [debug] 20090#20090: *14 find config phase: 1 (NGX_HTTP_FIND_CONFIG_PHASE), uri:/download/nginx-1.9.2.rar
2016/01/07 12:38:01[ ngx_http_core_find_static_location,  2907]  [debug] 20090#20090: *14 static_locations test location: "/", client uri:/download/nginx-1.9.2.rar
2016/01/07 12:38:01[ ngx_http_core_find_static_location,  2907]  [debug] 20090#20090: *14 static_locations test location: "mytest_upstream", client uri:/download/nginx-1.9.2.rar
2016/01/07 12:38:01[ ngx_http_core_find_static_location,  2907]  [debug] 20090#20090: *14 static_locations test location: "mytest_sendfile", client uri:/download/nginx-1.9.2.rar
2016/01/07 12:38:01[ ngx_http_core_find_static_location,  2907]  [debug] 20090#20090: *14 static_locations test location: "list", client uri:/download/nginx-1.9.2.rar
2016/01/07 12:38:01[        ngx_http_core_find_location,  2847]  [debug] 20090#20090: *14 ngx pcre test location: ~ "/download/"
2016/01/07 12:38:01[    ngx_http_core_find_config_phase,  2040]  [debug] 20090#20090: *14 using configuration "/download/"
2016/01/07 12:38:01[    ngx_http_core_find_config_phase,  2047]  [debug] 20090#20090: *14 http cl:-1 max:1048576, rc:0
2016/01/07 12:38:01[        ngx_http_core_rewrite_phase,  1962]  [debug] 20090#20090: *14 rewrite phase: 2 (NGX_HTTP_REWRITE_PHASE)
2016/01/07 12:38:01[   ngx_http_core_post_rewrite_phase,  2115]  [debug] 20090#20090: *14 post rewrite phase: 3 (NGX_HTTP_POST_REWRITE_PHASE)
2016/01/07 12:38:01[        ngx_http_core_generic_phase,  1898]  [debug] 20090#20090: *14 generic phase: 4 (NGX_HTTP_PREACCESS_PHASE)
2016/01/07 12:38:01[        ngx_http_core_generic_phase,  1898]  [debug] 20090#20090: *14 generic phase: 5 (NGX_HTTP_PREACCESS_PHASE)
2016/01/07 12:38:01[         ngx_http_core_access_phase,  2213]  [debug] 20090#20090: *14 access phase: 6 (NGX_HTTP_ACCESS_PHASE)
2016/01/07 12:38:01[         ngx_http_core_access_phase,  2213]  [debug] 20090#20090: *14 access phase: 7 (NGX_HTTP_ACCESS_PHASE)
2016/01/07 12:38:01[    ngx_http_core_post_access_phase,  2315]  [debug] 20090#20090: *14 post access phase: 8 (NGX_HTTP_POST_ACCESS_PHASE)
2016/01/07 12:38:01[        ngx_http_core_content_phase,  2645]  [debug] 20090#20090: *14 content phase: 9 (NGX_HTTP_CONTENT_PHASE)
2016/01/07 12:38:01[        ngx_http_core_content_phase,  2645]  [debug] 20090#20090: *14 content phase: 10 (NGX_HTTP_CONTENT_PHASE)
2016/01/07 12:38:01[        ngx_http_core_content_phase,  2645]  [debug] 20090#20090: *14 content phase: 11 (NGX_HTTP_CONTENT_PHASE)
2016/01/07 12:38:01[            ngx_http_static_handler,    85]  [debug] 20090#20090: *14 http filename: "/var/yyz/www/download/nginx-1.9.2.rar"
2016/01/07 12:38:01[            ngx_http_static_handler,   145]  [debug] 20090#20090: *14 http static fd: 13
2016/01/07 12:38:01[      ngx_http_discard_request_body,   796]  [debug] 20090#20090: *14 http set discard body
2016/01/07 12:38:01[               ngx_http_send_header,  3319][yangya  [debug] 20090#20090: *14 ngx http send header
2016/01/07 12:38:01[             ngx_http_header_filter,   677]  [debug] 20090#20090: *14 HTTP/1.1 200 OK
Server: nginx/1.9.2
Date: Thu, 07 Jan 2016 04:38:01 GMT
Content-Type: application/x-rar-compressed
Content-Length: 5114148
Last-Modified: Sun, 10 Jan 2016 07:14:02 GMT
Connection: keep-alive
ETag: "569204ba-4e0924"
Accept-Ranges: bytes

2016/01/07 12:38:01[              ngx_http_write_filter,   208]  [debug] 20090#20090: *14 write new buf temporary:1 buf-in-file:0, buf->start:080F3238, buf->pos:080F3238, buf_size: 263 file_pos: 0, in_file_size: 0
2016/01/07 12:38:01[              ngx_http_write_filter,   248]  [debug] 20090#20090: *14 http write filter: last:0 flush:0 size:263
2016/01/07 12:38:01[              ngx_http_write_filter,   290][yangya  [debug] 20090#20090: *14 send size:263 < min postpone_output:1460, do not send
2016/01/07 12:38:01[             ngx_http_output_filter,  3372]  [debug] 20090#20090: *14 http output filter "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[               ngx_http_copy_filter,   206]  [debug] 20090#20090: *14 http copy filter: "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931", r->aio:0
2016/01/07 12:38:01[                   ngx_output_chain,    67][yangya  [debug] 20090#20090: *14 ctx->sendfile:0, ctx->aio:0, ctx->directio:0
2016/01/07 12:38:01[             ngx_output_chain_as_is,   309][yangya  [debug] 20090#20090: ngx_output_chain_as_is--- buf_special:0, in_file:1, directio:1, buf_in_mem:0,need_in_memory:0, need_in_temp:0, memory:0, mmap:0
2016/01/07 12:38:01[             ngx_output_chain_as_is,   309][yangya  [debug] 20090#20090: ngx_output_chain_as_is--- buf_special:0, in_file:1, directio:1, buf_in_mem:0,need_in_memory:0, need_in_temp:0, memory:0, mmap:0
2016/01/07 12:38:01[                    ngx_thread_read,   147]  [debug] 20090#20090: *14 thread read: fd:13, buf:08115C00, size:32768, offset:0
2016/01/07 12:38:01[              ngx_thread_mutex_lock,   145]  [debug] 20090#20090: pthread_mutex_lock(080F03B0) enter
2016/01/07 12:38:01[               ngx_thread_task_post,   280][yangya  [debug] 20090#20090: ngx add task to thread, task id:183
2016/01/07 12:38:01[             ngx_thread_cond_signal,    54]  [debug] 20090#20090: pthread_cond_signal(080F03D4)
2016/01/07 12:38:01[               ngx_thread_cond_wait,    96]  [debug] 20090#20094: pthread_cond_wait(080F03D4) exit
2016/01/07 12:38:01[            ngx_thread_mutex_unlock,   171]  [debug] 20090#20094: pthread_mutex_unlock(080F03B0) exit
2016/01/07 12:38:01[              ngx_thread_pool_cycle,   370]  [debug] 20090#20094: run task #183 in thread pool name:"yang_pool"
2016/01/07 12:38:01[            ngx_thread_read_handler,   201]  [debug] 20090#20094: thread read handler
2016/01/07 12:38:01[            ngx_thread_mutex_unlock,   171]  [debug] 20090#20090: pthread_mutex_unlock(080F03B0) exit
2016/01/07 12:38:01[               ngx_thread_task_post,   297]  [debug] 20090#20090: task #183 added to thread pool name: "yang_pool" complete
2016/01/07 12:38:01[               ngx_http_copy_filter,   284]  [debug] 20090#20090: *14 http copy filter rc: -2, buffered:4 "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[          ngx_http_finalize_request,  2592]  [debug] 20090#20090: *14 http finalize request rc: -2, "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931" a:1, c:1
2016/01/07 12:38:01[                ngx_event_add_timer,   100]  [debug] 20090#20090: *14 <ngx_http_set_write_handler,  3023>  event timer add fd:12, expire-time:60 s, timer.key:442594977
2016/01/07 12:38:01[           ngx_event_process_posted,    65]  [debug] 20090#20090: begin to run befor posted event AE993098
2016/01/07 12:38:01[           ngx_event_process_posted,    67]  [debug] 20090#20090: *14 delete posted event AE993098
2016/01/07 12:38:01[           ngx_http_request_handler,  2401]  [debug] 20090#20090: *14 http run request(ev->write:1): "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                    ngx_http_writer,  3052]  [debug] 20090#20090: *14 http writer handler: "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                    ngx_http_writer,  3101]  [debug] 20090#20090: *14 http writer delayed
2016/01/07 12:38:01[           ngx_trylock_accept_mutex,   405]  [debug] 20090#20090: accept mutex locked
2016/01/07 12:38:01[            ngx_thread_read_handler,   219]  [debug] 20090#20094: pread: 32768 (err: 0) of 32768 @0
2016/01/07 12:38:01[              ngx_thread_pool_cycle,   376]  [debug] 20090#20094: complete task #183 in thread pool name: "yang_pool"
2016/01/07 12:38:01[              ngx_thread_mutex_lock,   145]  [debug] 20090#20094: pthread_mutex_lock(080F03B0) enter
2016/01/07 12:38:01[               ngx_thread_cond_wait,    70]  [debug] 20090#20094: pthread_cond_wait(080F03D4) enter
2016/01/07 12:38:01[           ngx_epoll_process_events,  1720]  [debug] 20090#20090: epoll: fd:9 EPOLLIN  (ev:0001) d:080E36C0
2016/01/07 12:38:01[           ngx_epoll_process_events,  1766]  [debug] 20090#20090: post event 080E3680
2016/01/07 12:38:01[           ngx_event_process_posted,    65]  [debug] 20090#20090: begin to run befor posted event 080E3680
2016/01/07 12:38:01[           ngx_event_process_posted,    67]  [debug] 20090#20090: delete posted event 080E3680
2016/01/07 12:38:01[            ngx_thread_pool_handler,   401]  [debug] 20090#20090: thread pool handler
2016/01/07 12:38:01[            ngx_thread_pool_handler,   422]  [debug] 20090#20090: run completion handler for task #183
2016/01/07 12:38:01[           ngx_http_request_handler,  2401]  [debug] 20090#20090: *14 http run request(ev->write:1): "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                    ngx_http_writer,  3052]  [debug] 20090#20090: *14 http writer handler: "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[             ngx_http_output_filter,  3372]  [debug] 20090#20090: *14 http output filter "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[               ngx_http_copy_filter,   206]  [debug] 20090#20090: *14 http copy filter: "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931", r->aio:0
2016/01/07 12:38:01[                   ngx_output_chain,    67][yangya  [debug] 20090#20090: *14 ctx->sendfile:0, ctx->aio:0, ctx->directio:1
2016/01/07 12:38:01[             ngx_output_chain_as_is,   309][yangya  [debug] 20090#20090: ngx_output_chain_as_is--- buf_special:0, in_file:1, directio:1, buf_in_mem:0,need_in_memory:0, need_in_temp:0, memory:0, mmap:0
2016/01/07 12:38:01[                    ngx_thread_read,   147]  [debug] 20090#20090: *14 thread read: fd:13, buf:08115C00, size:32768, offset:0
2016/01/07 12:38:01[             ngx_output_chain_as_is,   309][yangya  [debug] 20090#20090: ngx_output_chain_as_is--- buf_special:0, in_file:1, directio:1, buf_in_mem:0,need_in_memory:0, need_in_temp:0, memory:0, mmap:0
2016/01/07 12:38:01[           ngx_http_postpone_filter,   176]  [debug] 20090#20090: *14 http postpone filter "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931" 080F3438
2016/01/07 12:38:01[              ngx_http_write_filter,   151]  [debug] 20090#20090: *14 write old buf t:1 f:0 080F3238, pos 080F3238, size: 263 file: 0, size: 0
2016/01/07 12:38:01[              ngx_http_write_filter,   208]  [debug] 20090#20090: *14 write new buf temporary:1 buf-in-file:0, buf->start:08115C00, buf->pos:08115C00, buf_size: 32768 file_pos: 0, in_file_size: 0
2016/01/07 12:38:01[              ngx_http_write_filter,   248]  [debug] 20090#20090: *14 http write filter: last:0 flush:1 size:33031
2016/01/07 12:38:01[              ngx_http_write_filter,   380]  [debug] 20090#20090: *14 http write filter limit 0
2016/01/07 12:38:01[           ngx_linux_sendfile_chain,   201][yangya  [debug] 20090#20090: *14 @@@@@@@@@@@@@@@@@@@@@@@begin ngx_linux_sendfile_chain @@@@@@@@@@@@@@@@@@@
2016/01/07 12:38:01[                         ngx_writev,   238]  [debug] 20090#20090: *14 writev: 12600 of 33031
2016/01/07 12:38:01[              ngx_http_write_filter,   386]  [debug] 20090#20090: *14 http write filter 080F3440
2016/01/07 12:38:01[             ngx_output_chain_as_is,   309][yangya  [debug] 20090#20090: ngx_output_chain_as_is--- buf_special:0, in_file:1, directio:1, buf_in_mem:0,need_in_memory:0, need_in_temp:0, memory:0, mmap:0
2016/01/07 12:38:01[               ngx_http_copy_filter,   284]  [debug] 20090#20090: *14 http copy filter rc: -2, buffered:4 "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                    ngx_http_writer,  3118]  [debug] 20090#20090: *14 http writer output filter: -2, "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                ngx_event_add_timer,    89]  [debug] 20090#20090: *14 <          ngx_http_writer,  3139>  event timer: 12, old: 442594977, new: 442595017, 
2016/01/07 12:38:01[           ngx_trylock_accept_mutex,   405]  [debug] 20090#20090: accept mutex locked
2016/01/07 12:38:01[           ngx_epoll_process_events,  1720]  [debug] 20090#20090: epoll: fd:12 EPOLLOUT  (ev:0004) d:B2593158
2016/01/07 12:38:01[           ngx_epoll_process_events,  1793]  [debug] 20090#20090: *14 post event AE993098
2016/01/07 12:38:01[           ngx_event_process_posted,    65]  [debug] 20090#20090: begin to run befor posted event AE993098
2016/01/07 12:38:01[           ngx_event_process_posted,    67]  [debug] 20090#20090: *14 delete posted event AE993098
2016/01/07 12:38:01[           ngx_http_request_handler,  2401]  [debug] 20090#20090: *14 http run request(ev->write:1): "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                    ngx_http_writer,  3052]  [debug] 20090#20090: *14 http writer handler: "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[             ngx_http_output_filter,  3372]  [debug] 20090#20090: *14 http output filter "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[               ngx_http_copy_filter,   206]  [debug] 20090#20090: *14 http copy filter: "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931", r->aio:0
2016/01/07 12:38:01[                   ngx_output_chain,    67][yangya  [debug] 20090#20090: *14 ctx->sendfile:0, ctx->aio:0, ctx->directio:1
2016/01/07 12:38:01[             ngx_output_chain_as_is,   309][yangya  [debug] 20090#20090: ngx_output_chain_as_is--- buf_special:0, in_file:1, directio:1, buf_in_mem:0,need_in_memory:0, need_in_temp:0, memory:0, mmap:0
2016/01/07 12:38:01[           ngx_http_postpone_filter,   176]  [debug] 20090#20090: *14 http postpone filter "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931" 00000000
2016/01/07 12:38:01[              ngx_http_write_filter,   151]  [debug] 20090#20090: *14 write old buf t:1 f:0 08115C00, pos 08118C31, size: 20431 file: 0, size: 0
2016/01/07 12:38:01[              ngx_http_write_filter,   248]  [debug] 20090#20090: *14 http write filter: last:0 flush:1 size:20431
2016/01/07 12:38:01[              ngx_http_write_filter,   380]  [debug] 20090#20090: *14 http write filter limit 0
2016/01/07 12:38:01[           ngx_linux_sendfile_chain,   201][yangya  [debug] 20090#20090: *14 @@@@@@@@@@@@@@@@@@@@@@@begin ngx_linux_sendfile_chain @@@@@@@@@@@@@@@@@@@
2016/01/07 12:38:01[                         ngx_writev,   238]  [debug] 20090#20090: *14 writev: 19600 of 20431
2016/01/07 12:38:01[              ngx_http_write_filter,   386]  [debug] 20090#20090: *14 http write filter 080F3440
2016/01/07 12:38:01[             ngx_output_chain_as_is,   309][yangya  [debug] 20090#20090: ngx_output_chain_as_is--- buf_special:0, in_file:1, directio:1, buf_in_mem:0,need_in_memory:0, need_in_temp:0, memory:0, mmap:0
2016/01/07 12:38:01[               ngx_http_copy_filter,   284]  [debug] 20090#20090: *14 http copy filter rc: -2, buffered:4 "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                    ngx_http_writer,  3118]  [debug] 20090#20090: *14 http writer output filter: -2, "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                ngx_event_add_timer,    89]  [debug] 20090#20090: *14 <          ngx_http_writer,  3139>  event timer: 12, old: 442594977, new: 442595018, 
2016/01/07 12:38:01[           ngx_trylock_accept_mutex,   405]  [debug] 20090#20090: accept mutex locked
2016/01/07 12:38:01[           ngx_epoll_process_events,  1720]  [debug] 20090#20090: epoll: fd:12 EPOLLOUT  (ev:0004) d:B2593158
2016/01/07 12:38:01[           ngx_epoll_process_events,  1793]  [debug] 20090#20090: *14 post event AE993098
2016/01/07 12:38:01[           ngx_event_process_posted,    65]  [debug] 20090#20090: begin to run befor posted event AE993098
2016/01/07 12:38:01[           ngx_event_process_posted,    67]  [debug] 20090#20090: *14 delete posted event AE993098
2016/01/07 12:38:01[           ngx_http_request_handler,  2401]  [debug] 20090#20090: *14 http run request(ev->write:1): "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                    ngx_http_writer,  3052]  [debug] 20090#20090: *14 http writer handler: "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[             ngx_http_output_filter,  3372]  [debug] 20090#20090: *14 http output filter "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[               ngx_http_copy_filter,   206]  [debug] 20090#20090: *14 http copy filter: "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931", r->aio:0
2016/01/07 12:38:01[                   ngx_output_chain,    67][yangya  [debug] 20090#20090: *14 ctx->sendfile:0, ctx->aio:0, ctx->directio:1
2016/01/07 12:38:01[             ngx_output_chain_as_is,   309][yangya  [debug] 20090#20090: ngx_output_chain_as_is--- buf_special:0, in_file:1, directio:1, buf_in_mem:0,need_in_memory:0, need_in_temp:0, memory:0, mmap:0
2016/01/07 12:38:01[           ngx_http_postpone_filter,   176]  [debug] 20090#20090: *14 http postpone filter "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931" 00000000
2016/01/07 12:38:01[              ngx_http_write_filter,   151]  [debug] 20090#20090: *14 write old buf t:1 f:0 08115C00, pos 0811D8C1, size: 831 file: 0, size: 0
2016/01/07 12:38:01[              ngx_http_write_filter,   248]  [debug] 20090#20090: *14 http write filter: last:0 flush:1 size:831
2016/01/07 12:38:01[              ngx_http_write_filter,   380]  [debug] 20090#20090: *14 http write filter limit 0
2016/01/07 12:38:01[           ngx_linux_sendfile_chain,   201][yangya  [debug] 20090#20090: *14 @@@@@@@@@@@@@@@@@@@@@@@begin ngx_linux_sendfile_chain @@@@@@@@@@@@@@@@@@@
2016/01/07 12:38:01[                         ngx_writev,   238]  [debug] 20090#20090: *14 writev: 831 of 831
2016/01/07 12:38:01[              ngx_http_write_filter,   386]  [debug] 20090#20090: *14 http write filter 00000000
2016/01/07 12:38:01[             ngx_output_chain_as_is,   309][yangya  [debug] 20090#20090: ngx_output_chain_as_is--- buf_special:0, in_file:1, directio:1, buf_in_mem:0,need_in_memory:0, need_in_temp:0, memory:0, mmap:0
2016/01/07 12:38:01[                    ngx_thread_read,   147]  [debug] 20090#20090: *14 thread read: fd:13, buf:08115C00, size:32768, offset:32768
2016/01/07 12:38:01[              ngx_thread_mutex_lock,   145]  [debug] 20090#20090: pthread_mutex_lock(080F03B0) enter
2016/01/07 12:38:01[               ngx_thread_task_post,   280][yangya  [debug] 20090#20090: ngx add task to thread, task id:184
2016/01/07 12:38:01[             ngx_thread_cond_signal,    54]  [debug] 20090#20090: pthread_cond_signal(080F03D4)
2016/01/07 12:38:01[               ngx_thread_cond_wait,    96]  [debug] 20090#20094: pthread_cond_wait(080F03D4) exit
2016/01/07 12:38:01[            ngx_thread_mutex_unlock,   171]  [debug] 20090#20094: pthread_mutex_unlock(080F03B0) exit
2016/01/07 12:38:01[              ngx_thread_pool_cycle,   370]  [debug] 20090#20094: run task #184 in thread pool name:"yang_pool"
2016/01/07 12:38:01[            ngx_thread_read_handler,   201]  [debug] 20090#20094: thread read handler
2016/01/07 12:38:01[            ngx_thread_mutex_unlock,   171]  [debug] 20090#20090: pthread_mutex_unlock(080F03B0) exit
2016/01/07 12:38:01[               ngx_thread_task_post,   297]  [debug] 20090#20090: task #184 added to thread pool name: "yang_pool" complete
2016/01/07 12:38:01[               ngx_http_copy_filter,   284]  [debug] 20090#20090: *14 http copy filter rc: -2, buffered:4 "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                    ngx_http_writer,  3118]  [debug] 20090#20090: *14 http writer output filter: -2, "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                ngx_event_add_timer,    89]  [debug] 20090#20090: *14 <          ngx_http_writer,  3139>  event timer: 12, old: 442594977, new: 442595020, 
2016/01/07 12:38:01[           ngx_trylock_accept_mutex,   405]  [debug] 20090#20090: accept mutex locked
2016/01/07 12:38:01[            ngx_thread_read_handler,   219]  [debug] 20090#20094: pread: 32768 (err: 0) of 32768 @32768
2016/01/07 12:38:01[              ngx_thread_pool_cycle,   376]  [debug] 20090#20094: complete task #184 in thread pool name: "yang_pool"
2016/01/07 12:38:01[              ngx_thread_mutex_lock,   145]  [debug] 20090#20094: pthread_mutex_lock(080F03B0) enter
2016/01/07 12:38:01[               ngx_thread_cond_wait,    70]  [debug] 20090#20094: pthread_cond_wait(080F03D4) enter
2016/01/07 12:38:01[           ngx_epoll_process_events,  1720]  [debug] 20090#20090: epoll: fd:9 EPOLLIN  (ev:0001) d:080E36C0
2016/01/07 12:38:01[           ngx_epoll_process_events,  1766]  [debug] 20090#20090: post event 080E3680
2016/01/07 12:38:01[           ngx_event_process_posted,    65]  [debug] 20090#20090: begin to run befor posted event 080E3680
2016/01/07 12:38:01[           ngx_event_process_posted,    67]  [debug] 20090#20090: delete posted event 080E3680
2016/01/07 12:38:01[            ngx_thread_pool_handler,   401]  [debug] 20090#20090: thread pool handler
2016/01/07 12:38:01[            ngx_thread_pool_handler,   422]  [debug] 20090#20090: run completion handler for task #184
2016/01/07 12:38:01[           ngx_http_request_handler,  2401]  [debug] 20090#20090: *14 http run request(ev->write:1): "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                    ngx_http_writer,  3052]  [debug] 20090#20090: *14 http writer handler: "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[             ngx_http_output_filter,  3372]  [debug] 20090#20090: *14 http output filter "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[               ngx_http_copy_filter,   206]  [debug] 20090#20090: *14 http copy filter: "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931", r->aio:0
2016/01/07 12:38:01[                   ngx_output_chain,    67][yangya  [debug] 20090#20090: *14 ctx->sendfile:0, ctx->aio:0, ctx->directio:1
2016/01/07 12:38:01[             ngx_output_chain_as_is,   309][yangya  [debug] 20090#20090: ngx_output_chain_as_is--- buf_special:0, in_file:1, directio:1, buf_in_mem:0,need_in_memory:0, need_in_temp:0, memory:0, mmap:0
2016/01/07 12:38:01[                    ngx_thread_read,   147]  [debug] 20090#20090: *14 thread read: fd:13, buf:08115C00, size:32768, offset:32768
2016/01/07 12:38:01[             ngx_output_chain_as_is,   309][yangya  [debug] 20090#20090: ngx_output_chain_as_is--- buf_special:0, in_file:1, directio:1, buf_in_mem:0,need_in_memory:0, need_in_temp:0, memory:0, mmap:0
2016/01/07 12:38:01[           ngx_http_postpone_filter,   176]  [debug] 20090#20090: *14 http postpone filter "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931" 080F3438
2016/01/07 12:38:01[              ngx_http_write_filter,   208]  [debug] 20090#20090: *14 write new buf temporary:1 buf-in-file:0, buf->start:08115C00, buf->pos:08115C00, buf_size: 32768 file_pos: 0, in_file_size: 0
2016/01/07 12:38:01[              ngx_http_write_filter,   248]  [debug] 20090#20090: *14 http write filter: last:0 flush:1 size:32768
2016/01/07 12:38:01[              ngx_http_write_filter,   380]  [debug] 20090#20090: *14 http write filter limit 0
2016/01/07 12:38:01[           ngx_linux_sendfile_chain,   201][yangya  [debug] 20090#20090: *14 @@@@@@@@@@@@@@@@@@@@@@@begin ngx_linux_sendfile_chain @@@@@@@@@@@@@@@@@@@
2016/01/07 12:38:01[                         ngx_writev,   238]  [debug] 20090#20090: *14 writev: 32768 of 32768
2016/01/07 12:38:01[              ngx_http_write_filter,   386]  [debug] 20090#20090: *14 http write filter 00000000
2016/01/07 12:38:01[             ngx_output_chain_as_is,   309][yangya  [debug] 20090#20090: ngx_output_chain_as_is--- buf_special:0, in_file:1, directio:1, buf_in_mem:0,need_in_memory:0, need_in_temp:0, memory:0, mmap:0
2016/01/07 12:38:01[                    ngx_thread_read,   147]  [debug] 20090#20090: *14 thread read: fd:13, buf:08115C00, size:32768, offset:65536
2016/01/07 12:38:01[              ngx_thread_mutex_lock,   145]  [debug] 20090#20090: pthread_mutex_lock(080F03B0) enter
2016/01/07 12:38:01[               ngx_thread_task_post,   280][yangya  [debug] 20090#20090: ngx add task to thread, task id:185
2016/01/07 12:38:01[             ngx_thread_cond_signal,    54]  [debug] 20090#20090: pthread_cond_signal(080F03D4)
2016/01/07 12:38:01[               ngx_thread_cond_wait,    96]  [debug] 20090#20094: pthread_cond_wait(080F03D4) exit
2016/01/07 12:38:01[            ngx_thread_mutex_unlock,   171]  [debug] 20090#20094: pthread_mutex_unlock(080F03B0) exit
2016/01/07 12:38:01[              ngx_thread_pool_cycle,   370]  [debug] 20090#20094: run task #185 in thread pool name:"yang_pool"
2016/01/07 12:38:01[            ngx_thread_read_handler,   201]  [debug] 20090#20094: thread read handler
2016/01/07 12:38:01[            ngx_thread_mutex_unlock,   171]  [debug] 20090#20090: pthread_mutex_unlock(080F03B0) exit
2016/01/07 12:38:01[               ngx_thread_task_post,   297]  [debug] 20090#20090: task #185 added to thread pool name: "yang_pool" complete
2016/01/07 12:38:01[               ngx_http_copy_filter,   284]  [debug] 20090#20090: *14 http copy filter rc: -2, buffered:4 "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                    ngx_http_writer,  3118]  [debug] 20090#20090: *14 http writer output filter: -2, "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                ngx_event_add_timer,    89]  [debug] 20090#20090: *14 <          ngx_http_writer,  3139>  event timer: 12, old: 442594977, new: 442595023, 
2016/01/07 12:38:01[           ngx_trylock_accept_mutex,   405]  [debug] 20090#20090: accept mutex locked
2016/01/07 12:38:01[            ngx_thread_read_handler,   219]  [debug] 20090#20094: pread: 32768 (err: 0) of 32768 @65536
2016/01/07 12:38:01[              ngx_thread_pool_cycle,   376]  [debug] 20090#20094: complete task #185 in thread pool name: "yang_pool"
2016/01/07 12:38:01[              ngx_thread_mutex_lock,   145]  [debug] 20090#20094: pthread_mutex_lock(080F03B0) enter
2016/01/07 12:38:01[               ngx_thread_cond_wait,    70]  [debug] 20090#20094: pthread_cond_wait(080F03D4) enter
2016/01/07 12:38:01[           ngx_epoll_process_events,  1720]  [debug] 20090#20090: epoll: fd:9 EPOLLIN  (ev:0001) d:080E36C0
2016/01/07 12:38:01[           ngx_epoll_process_events,  1766]  [debug] 20090#20090: post event 080E3680
2016/01/07 12:38:01[           ngx_event_process_posted,    65]  [debug] 20090#20090: begin to run befor posted event 080E3680
2016/01/07 12:38:01[           ngx_event_process_posted,    67]  [debug] 20090#20090: delete posted event 080E3680
2016/01/07 12:38:01[            ngx_thread_pool_handler,   401]  [debug] 20090#20090: thread pool handler
2016/01/07 12:38:01[            ngx_thread_pool_handler,   422]  [debug] 20090#20090: run completion handler for task #185
2016/01/07 12:38:01[           ngx_http_request_handler,  2401]  [debug] 20090#20090: *14 http run request(ev->write:1): "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                    ngx_http_writer,  3052]  [debug] 20090#20090: *14 http writer handler: "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[             ngx_http_output_filter,  3372]  [debug] 20090#20090: *14 http output filter "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[               ngx_http_copy_filter,   206]  [debug] 20090#20090: *14 http copy filter: "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931", r->aio:0
2016/01/07 12:38:01[                   ngx_output_chain,    67][yangya  [debug] 20090#20090: *14 ctx->sendfile:0, ctx->aio:0, ctx->directio:1
2016/01/07 12:38:01[             ngx_output_chain_as_is,   309][yangya  [debug] 20090#20090: ngx_output_chain_as_is--- buf_special:0, in_file:1, directio:1, buf_in_mem:0,need_in_memory:0, need_in_temp:0, memory:0, mmap:0
2016/01/07 12:38:01[                    ngx_thread_read,   147]  [debug] 20090#20090: *14 thread read: fd:13, buf:08115C00, size:32768, offset:65536
2016/01/07 12:38:01[             ngx_output_chain_as_is,   309][yangya  [debug] 20090#20090: ngx_output_chain_as_is--- buf_special:0, in_file:1, directio:1, buf_in_mem:0,need_in_memory:0, need_in_temp:0, memory:0, mmap:0
2016/01/07 12:38:01[           ngx_http_postpone_filter,   176]  [debug] 20090#20090: *14 http postpone filter "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931" 080F3438
2016/01/07 12:38:01[              ngx_http_write_filter,   208]  [debug] 20090#20090: *14 write new buf temporary:1 buf-in-file:0, buf->start:08115C00, buf->pos:08115C00, buf_size: 32768 file_pos: 0, in_file_size: 0
2016/01/07 12:38:01[              ngx_http_write_filter,   248]  [debug] 20090#20090: *14 http write filter: last:0 flush:1 size:32768
2016/01/07 12:38:01[              ngx_http_write_filter,   380]  [debug] 20090#20090: *14 http write filter limit 0
2016/01/07 12:38:01[           ngx_linux_sendfile_chain,   201][yangya  [debug] 20090#20090: *14 @@@@@@@@@@@@@@@@@@@@@@@begin ngx_linux_sendfile_chain @@@@@@@@@@@@@@@@@@@
2016/01/07 12:38:01[                         ngx_writev,   238]  [debug] 20090#20090: *14 writev: 29400 of 32768
2016/01/07 12:38:01[              ngx_http_write_filter,   386]  [debug] 20090#20090: *14 http write filter 080F3440
2016/01/07 12:38:01[             ngx_output_chain_as_is,   309][yangya  [debug] 20090#20090: ngx_output_chain_as_is--- buf_special:0, in_file:1, directio:1, buf_in_mem:0,need_in_memory:0, need_in_temp:0, memory:0, mmap:0
2016/01/07 12:38:01[               ngx_http_copy_filter,   284]  [debug] 20090#20090: *14 http copy filter rc: -2, buffered:4 "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                    ngx_http_writer,  3118]  [debug] 20090#20090: *14 http writer output filter: -2, "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                ngx_event_add_timer,    89]  [debug] 20090#20090: *14 <          ngx_http_writer,  3139>  event timer: 12, old: 442594977, new: 442595027, 
2016/01/07 12:38:01[           ngx_trylock_accept_mutex,   405]  [debug] 20090#20090: accept mutex locked
2016/01/07 12:38:01[           ngx_epoll_process_events,  1720]  [debug] 20090#20090: epoll: fd:12 EPOLLOUT  (ev:0004) d:B2593158
2016/01/07 12:38:01[           ngx_epoll_process_events,  1793]  [debug] 20090#20090: *14 post event AE993098
2016/01/07 12:38:01[           ngx_event_process_posted,    65]  [debug] 20090#20090: begin to run befor posted event AE993098
2016/01/07 12:38:01[           ngx_event_process_posted,    67]  [debug] 20090#20090: *14 delete posted event AE993098
2016/01/07 12:38:01[           ngx_http_request_handler,  2401]  [debug] 20090#20090: *14 http run request(ev->write:1): "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                    ngx_http_writer,  3052]  [debug] 20090#20090: *14 http writer handler: "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[             ngx_http_output_filter,  3372]  [debug] 20090#20090: *14 http output filter "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[               ngx_http_copy_filter,   206]  [debug] 20090#20090: *14 http copy filter: "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931", r->aio:0
2016/01/07 12:38:01[                   ngx_output_chain,    67][yangya  [debug] 20090#20090: *14 ctx->sendfile:0, ctx->aio:0, ctx->directio:1
2016/01/07 12:38:01[             ngx_output_chain_as_is,   309][yangya  [debug] 20090#20090: ngx_output_chain_as_is--- buf_special:0, in_file:1, directio:1, buf_in_mem:0,need_in_memory:0, need_in_temp:0, memory:0, mmap:0
2016/01/07 12:38:01[           ngx_http_postpone_filter,   176]  [debug] 20090#20090: *14 http postpone filter "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931" 00000000
2016/01/07 12:38:01[              ngx_http_write_filter,   151]  [debug] 20090#20090: *14 write old buf t:1 f:0 08115C00, pos 0811CED8, size: 3368 file: 0, size: 0
2016/01/07 12:38:01[              ngx_http_write_filter,   248]  [debug] 20090#20090: *14 http write filter: last:0 flush:1 size:3368
2016/01/07 12:38:01[              ngx_http_write_filter,   380]  [debug] 20090#20090: *14 http write filter limit 0
2016/01/07 12:38:01[           ngx_linux_sendfile_chain,   201][yangya  [debug] 20090#20090: *14 @@@@@@@@@@@@@@@@@@@@@@@begin ngx_linux_sendfile_chain @@@@@@@@@@@@@@@@@@@
2016/01/07 12:38:01[                         ngx_writev,   238]  [debug] 20090#20090: *14 writev: 3368 of 3368
2016/01/07 12:38:01[              ngx_http_write_filter,   386]  [debug] 20090#20090: *14 http write filter 00000000
2016/01/07 12:38:01[             ngx_output_chain_as_is,   309][yangya  [debug] 20090#20090: ngx_output_chain_as_is--- buf_special:0, in_file:1, directio:1, buf_in_mem:0,need_in_memory:0, need_in_temp:0, memory:0, mmap:0
2016/01/07 12:38:01[                    ngx_thread_read,   147]  [debug] 20090#20090: *14 thread read: fd:13, buf:08115C00, size:32768, offset:98304
2016/01/07 12:38:01[              ngx_thread_mutex_lock,   145]  [debug] 20090#20090: pthread_mutex_lock(080F03B0) enter
2016/01/07 12:38:01[               ngx_thread_task_post,   280][yangya  [debug] 20090#20090: ngx add task to thread, task id:186
2016/01/07 12:38:01[             ngx_thread_cond_signal,    54]  [debug] 20090#20090: pthread_cond_signal(080F03D4)
2016/01/07 12:38:01[               ngx_thread_cond_wait,    96]  [debug] 20090#20094: pthread_cond_wait(080F03D4) exit
2016/01/07 12:38:01[            ngx_thread_mutex_unlock,   171]  [debug] 20090#20094: pthread_mutex_unlock(080F03B0) exit
2016/01/07 12:38:01[              ngx_thread_pool_cycle,   370]  [debug] 20090#20094: run task #186 in thread pool name:"yang_pool"
2016/01/07 12:38:01[            ngx_thread_read_handler,   201]  [debug] 20090#20094: thread read handler
2016/01/07 12:38:01[            ngx_thread_read_handler,   219]  [debug] 20090#20094: pread: 32768 (err: 0) of 32768 @98304
2016/01/07 12:38:01[              ngx_thread_pool_cycle,   376]  [debug] 20090#20094: complete task #186 in thread pool name: "yang_pool"
2016/01/07 12:38:01[              ngx_thread_mutex_lock,   145]  [debug] 20090#20094: pthread_mutex_lock(080F03B0) enter
2016/01/07 12:38:01[               ngx_thread_cond_wait,    70]  [debug] 20090#20094: pthread_cond_wait(080F03D4) enter
2016/01/07 12:38:01[            ngx_thread_mutex_unlock,   171]  [debug] 20090#20090: pthread_mutex_unlock(080F03B0) exit
2016/01/07 12:38:01[               ngx_thread_task_post,   297]  [debug] 20090#20090: task #186 added to thread pool name: "yang_pool" complete
2016/01/07 12:38:01[               ngx_http_copy_filter,   284]  [debug] 20090#20090: *14 http copy filter rc: -2, buffered:4 "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                    ngx_http_writer,  3118]  [debug] 20090#20090: *14 http writer output filter: -2, "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                ngx_event_add_timer,    89]  [debug] 20090#20090: *14 <          ngx_http_writer,  3139>  event timer: 12, old: 442594977, new: 442595029, 
2016/01/07 12:38:01[           ngx_trylock_accept_mutex,   405]  [debug] 20090#20090: accept mutex locked
2016/01/07 12:38:01[           ngx_epoll_process_events,  1720]  [debug] 20090#20090: epoll: fd:9 EPOLLIN  (ev:0001) d:080E36C0
2016/01/07 12:38:01[           ngx_epoll_process_events,  1766]  [debug] 20090#20090: post event 080E3680
2016/01/07 12:38:01[           ngx_event_process_posted,    65]  [debug] 20090#20090: begin to run befor posted event 080E3680
2016/01/07 12:38:01[           ngx_event_process_posted,    67]  [debug] 20090#20090: delete posted event 080E3680
2016/01/07 12:38:01[            ngx_thread_pool_handler,   401]  [debug] 20090#20090: thread pool handler
2016/01/07 12:38:01[            ngx_thread_pool_handler,   422]  [debug] 20090#20090: run completion handler for task #186
2016/01/07 12:38:01[           ngx_http_request_handler,  2401]  [debug] 20090#20090: *14 http run request(ev->write:1): "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                    ngx_http_writer,  3052]  [debug] 20090#20090: *14 http writer handler: "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[             ngx_http_output_filter,  3372]  [debug] 20090#20090: *14 http output filter "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[               ngx_http_copy_filter,   206]  [debug] 20090#20090: *14 http copy filter: "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931", r->aio:0
2016/01/07 12:38:01[                   ngx_output_chain,    67][yangya  [debug] 20090#20090: *14 ctx->sendfile:0, ctx->aio:0, ctx->directio:1
2016/01/07 12:38:01[             ngx_output_chain_as_is,   309][yangya  [debug] 20090#20090: ngx_output_chain_as_is--- buf_special:0, in_file:1, directio:1, buf_in_mem:0,need_in_memory:0, need_in_temp:0, memory:0, mmap:0
2016/01/07 12:38:01[                    ngx_thread_read,   147]  [debug] 20090#20090: *14 thread read: fd:13, buf:08115C00, size:32768, offset:98304
2016/01/07 12:38:01[             ngx_output_chain_as_is,   309][yangya  [debug] 20090#20090: ngx_output_chain_as_is--- buf_special:0, in_file:1, directio:1, buf_in_mem:0,need_in_memory:0, need_in_temp:0, memory:0, mmap:0
2016/01/07 12:38:01[           ngx_http_postpone_filter,   176]  [debug] 20090#20090: *14 http postpone filter "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931" 080F3438
2016/01/07 12:38:01[              ngx_http_write_filter,   208]  [debug] 20090#20090: *14 write new buf temporary:1 buf-in-file:0, buf->start:08115C00, buf->pos:08115C00, buf_size: 32768 file_pos: 0, in_file_size: 0
2016/01/07 12:38:01[              ngx_http_write_filter,   248]  [debug] 20090#20090: *14 http write filter: last:0 flush:1 size:32768
2016/01/07 12:38:01[              ngx_http_write_filter,   380]  [debug] 20090#20090: *14 http write filter limit 0
2016/01/07 12:38:01[           ngx_linux_sendfile_chain,   201][yangya  [debug] 20090#20090: *14 @@@@@@@@@@@@@@@@@@@@@@@begin ngx_linux_sendfile_chain @@@@@@@@@@@@@@@@@@@
2016/01/07 12:38:01[                         ngx_writev,   238]  [debug] 20090#20090: *14 writev: 32768 of 32768
2016/01/07 12:38:01[              ngx_http_write_filter,   386]  [debug] 20090#20090: *14 http write filter 00000000
2016/01/07 12:38:01[             ngx_output_chain_as_is,   309][yangya  [debug] 20090#20090: ngx_output_chain_as_is--- buf_special:0, in_file:1, directio:1, buf_in_mem:0,need_in_memory:0, need_in_temp:0, memory:0, mmap:0
2016/01/07 12:38:01[                    ngx_thread_read,   147]  [debug] 20090#20090: *14 thread read: fd:13, buf:08115C00, size:32768, offset:131072
2016/01/07 12:38:01[              ngx_thread_mutex_lock,   145]  [debug] 20090#20090: pthread_mutex_lock(080F03B0) enter
2016/01/07 12:38:01[               ngx_thread_task_post,   280][yangya  [debug] 20090#20090: ngx add task to thread, task id:187
2016/01/07 12:38:01[             ngx_thread_cond_signal,    54]  [debug] 20090#20090: pthread_cond_signal(080F03D4)
2016/01/07 12:38:01[               ngx_thread_cond_wait,    96]  [debug] 20090#20094: pthread_cond_wait(080F03D4) exit
2016/01/07 12:38:01[            ngx_thread_mutex_unlock,   171]  [debug] 20090#20094: pthread_mutex_unlock(080F03B0) exit
2016/01/07 12:38:01[              ngx_thread_pool_cycle,   370]  [debug] 20090#20094: run task #187 in thread pool name:"yang_pool"
2016/01/07 12:38:01[            ngx_thread_read_handler,   201]  [debug] 20090#20094: thread read handler
2016/01/07 12:38:01[            ngx_thread_mutex_unlock,   171]  [debug] 20090#20090: pthread_mutex_unlock(080F03B0) exit
2016/01/07 12:38:01[               ngx_thread_task_post,   297]  [debug] 20090#20090: task #187 added to thread pool name: "yang_pool" complete
2016/01/07 12:38:01[               ngx_http_copy_filter,   284]  [debug] 20090#20090: *14 http copy filter rc: -2, buffered:4 "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                    ngx_http_writer,  3118]  [debug] 20090#20090: *14 http writer output filter: -2, "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                ngx_event_add_timer,    89]  [debug] 20090#20090: *14 <          ngx_http_writer,  3139>  event timer: 12, old: 442594977, new: 442595030, 
2016/01/07 12:38:01[           ngx_trylock_accept_mutex,   405]  [debug] 20090#20090: accept mutex locked
2016/01/07 12:38:01[            ngx_thread_read_handler,   219]  [debug] 20090#20094: pread: 32768 (err: 0) of 32768 @131072
2016/01/07 12:38:01[              ngx_thread_pool_cycle,   376]  [debug] 20090#20094: complete task #187 in thread pool name: "yang_pool"
2016/01/07 12:38:01[              ngx_thread_mutex_lock,   145]  [debug] 20090#20094: pthread_mutex_lock(080F03B0) enter
2016/01/07 12:38:01[               ngx_thread_cond_wait,    70]  [debug] 20090#20094: pthread_cond_wait(080F03D4) enter
2016/01/07 12:38:01[           ngx_epoll_process_events,  1720]  [debug] 20090#20090: epoll: fd:9 EPOLLIN  (ev:0001) d:080E36C0
2016/01/07 12:38:01[           ngx_epoll_process_events,  1766]  [debug] 20090#20090: post event 080E3680
2016/01/07 12:38:01[           ngx_event_process_posted,    65]  [debug] 20090#20090: begin to run befor posted event 080E3680
2016/01/07 12:38:01[           ngx_event_process_posted,    67]  [debug] 20090#20090: delete posted event 080E3680
2016/01/07 12:38:01[            ngx_thread_pool_handler,   401]  [debug] 20090#20090: thread pool handler
2016/01/07 12:38:01[            ngx_thread_pool_handler,   422]  [debug] 20090#20090: run completion handler for task #187
2016/01/07 12:38:01[           ngx_http_request_handler,  2401]  [debug] 20090#20090: *14 http run request(ev->write:1): "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                    ngx_http_writer,  3052]  [debug] 20090#20090: *14 http writer handler: "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[             ngx_http_output_filter,  3372]  [debug] 20090#20090: *14 http output filter "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[               ngx_http_copy_filter,   206]  [debug] 20090#20090: *14 http copy filter: "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931", r->aio:0
2016/01/07 12:38:01[                   ngx_output_chain,    67][yangya  [debug] 20090#20090: *14 ctx->sendfile:0, ctx->aio:0, ctx->directio:1
2016/01/07 12:38:01[             ngx_output_chain_as_is,   309][yangya  [debug] 20090#20090: ngx_output_chain_as_is--- buf_special:0, in_file:1, directio:1, buf_in_mem:0,need_in_memory:0, need_in_temp:0, memory:0, mmap:0
2016/01/07 12:38:01[                    ngx_thread_read,   147]  [debug] 20090#20090: *14 thread read: fd:13, buf:08115C00, size:32768, offset:131072
2016/01/07 12:38:01[             ngx_output_chain_as_is,   309][yangya  [debug] 20090#20090: ngx_output_chain_as_is--- buf_special:0, in_file:1, directio:1, buf_in_mem:0,need_in_memory:0, need_in_temp:0, memory:0, mmap:0
2016/01/07 12:38:01[           ngx_http_postpone_filter,   176]  [debug] 20090#20090: *14 http postpone filter "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931" 080F3438
2016/01/07 12:38:01[              ngx_http_write_filter,   208]  [debug] 20090#20090: *14 write new buf temporary:1 buf-in-file:0, buf->start:08115C00, buf->pos:08115C00, buf_size: 32768 file_pos: 0, in_file_size: 0
2016/01/07 12:38:01[              ngx_http_write_filter,   248]  [debug] 20090#20090: *14 http write filter: last:0 flush:1 size:32768
2016/01/07 12:38:01[              ngx_http_write_filter,   380]  [debug] 20090#20090: *14 http write filter limit 0
2016/01/07 12:38:01[           ngx_linux_sendfile_chain,   201][yangya  [debug] 20090#20090: *14 @@@@@@@@@@@@@@@@@@@@@@@begin ngx_linux_sendfile_chain @@@@@@@@@@@@@@@@@@@
2016/01/07 12:38:01[                         ngx_writev,   238]  [debug] 20090#20090: *14 writev: 17632 of 32768
2016/01/07 12:38:01[              ngx_http_write_filter,   386]  [debug] 20090#20090: *14 http write filter 080F3440
2016/01/07 12:38:01[             ngx_output_chain_as_is,   309][yangya  [debug] 20090#20090: ngx_output_chain_as_is--- buf_special:0, in_file:1, directio:1, buf_in_mem:0,need_in_memory:0, need_in_temp:0, memory:0, mmap:0
2016/01/07 12:38:01[               ngx_http_copy_filter,   284]  [debug] 20090#20090: *14 http copy filter rc: -2, buffered:4 "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                    ngx_http_writer,  3118]  [debug] 20090#20090: *14 http writer output filter: -2, "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                ngx_event_add_timer,    89]  [debug] 20090#20090: *14 <          ngx_http_writer,  3139>  event timer: 12, old: 442594977, new: 442595037, 
2016/01/07 12:38:01[           ngx_trylock_accept_mutex,   405]  [debug] 20090#20090: accept mutex locked
2016/01/07 12:38:01[           ngx_epoll_process_events,  1720]  [debug] 20090#20090: epoll: fd:12 EPOLLOUT  (ev:0004) d:B2593158
2016/01/07 12:38:01[           ngx_epoll_process_events,  1793]  [debug] 20090#20090: *14 post event AE993098
2016/01/07 12:38:01[           ngx_event_process_posted,    65]  [debug] 20090#20090: begin to run befor posted event AE993098
2016/01/07 12:38:01[           ngx_event_process_posted,    67]  [debug] 20090#20090: *14 delete posted event AE993098
2016/01/07 12:38:01[           ngx_http_request_handler,  2401]  [debug] 20090#20090: *14 http run request(ev->write:1): "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                    ngx_http_writer,  3052]  [debug] 20090#20090: *14 http writer handler: "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[             ngx_http_output_filter,  3372]  [debug] 20090#20090: *14 http output filter "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[               ngx_http_copy_filter,   206]  [debug] 20090#20090: *14 http copy filter: "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931", r->aio:0
2016/01/07 12:38:01[                   ngx_output_chain,    67][yangya  [debug] 20090#20090: *14 ctx->sendfile:0, ctx->aio:0, ctx->directio:1
2016/01/07 12:38:01[             ngx_output_chain_as_is,   309][yangya  [debug] 20090#20090: ngx_output_chain_as_is--- buf_special:0, in_file:1, directio:1, buf_in_mem:0,need_in_memory:0, need_in_temp:0, memory:0, mmap:0
2016/01/07 12:38:01[           ngx_http_postpone_filter,   176]  [debug] 20090#20090: *14 http postpone filter "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931" 00000000
2016/01/07 12:38:01[              ngx_http_write_filter,   151]  [debug] 20090#20090: *14 write old buf t:1 f:0 08115C00, pos 0811A0E0, size: 15136 file: 0, size: 0
2016/01/07 12:38:01[              ngx_http_write_filter,   248]  [debug] 20090#20090: *14 http write filter: last:0 flush:1 size:15136
2016/01/07 12:38:01[              ngx_http_write_filter,   380]  [debug] 20090#20090: *14 http write filter limit 0
2016/01/07 12:38:01[           ngx_linux_sendfile_chain,   201][yangya  [debug] 20090#20090: *14 @@@@@@@@@@@@@@@@@@@@@@@begin ngx_linux_sendfile_chain @@@@@@@@@@@@@@@@@@@
2016/01/07 12:38:01[                         ngx_writev,   238]  [debug] 20090#20090: *14 writev: 15136 of 15136
2016/01/07 12:38:01[              ngx_http_write_filter,   386]  [debug] 20090#20090: *14 http write filter 00000000
2016/01/07 12:38:01[             ngx_output_chain_as_is,   309][yangya  [debug] 20090#20090: ngx_output_chain_as_is--- buf_special:0, in_file:1, directio:1, buf_in_mem:0,need_in_memory:0, need_in_temp:0, memory:0, mmap:0
2016/01/07 12:38:01[                    ngx_thread_read,   147]  [debug] 20090#20090: *14 thread read: fd:13, buf:08115C00, size:32768, offset:163840
2016/01/07 12:38:01[              ngx_thread_mutex_lock,   145]  [debug] 20090#20090: pthread_mutex_lock(080F03B0) enter
2016/01/07 12:38:01[               ngx_thread_task_post,   280][yangya  [debug] 20090#20090: ngx add task to thread, task id:188
2016/01/07 12:38:01[             ngx_thread_cond_signal,    54]  [debug] 20090#20090: pthread_cond_signal(080F03D4)
2016/01/07 12:38:01[               ngx_thread_cond_wait,    96]  [debug] 20090#20094: pthread_cond_wait(080F03D4) exit
2016/01/07 12:38:01[            ngx_thread_mutex_unlock,   171]  [debug] 20090#20094: pthread_mutex_unlock(080F03B0) exit
2016/01/07 12:38:01[              ngx_thread_pool_cycle,   370]  [debug] 20090#20094: run task #188 in thread pool name:"yang_pool"
2016/01/07 12:38:01[            ngx_thread_read_handler,   201]  [debug] 20090#20094: thread read handler
2016/01/07 12:38:01[            ngx_thread_read_handler,   219]  [debug] 20090#20094: pread: 32768 (err: 0) of 32768 @163840
2016/01/07 12:38:01[              ngx_thread_pool_cycle,   376]  [debug] 20090#20094: complete task #188 in thread pool name: "yang_pool"
2016/01/07 12:38:01[              ngx_thread_mutex_lock,   145]  [debug] 20090#20094: pthread_mutex_lock(080F03B0) enter
2016/01/07 12:38:01[               ngx_thread_cond_wait,    70]  [debug] 20090#20094: pthread_cond_wait(080F03D4) enter
2016/01/07 12:38:01[            ngx_thread_mutex_unlock,   171]  [debug] 20090#20090: pthread_mutex_unlock(080F03B0) exit
2016/01/07 12:38:01[               ngx_thread_task_post,   297]  [debug] 20090#20090: task #188 added to thread pool name: "yang_pool" complete
2016/01/07 12:38:01[               ngx_http_copy_filter,   284]  [debug] 20090#20090: *14 http copy filter rc: -2, buffered:4 "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                    ngx_http_writer,  3118]  [debug] 20090#20090: *14 http writer output filter: -2, "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                ngx_event_add_timer,    89]  [debug] 20090#20090: *14 <          ngx_http_writer,  3139>  event timer: 12, old: 442594977, new: 442595066, 
2016/01/07 12:38:01[           ngx_trylock_accept_mutex,   405]  [debug] 20090#20090: accept mutex locked
2016/01/07 12:38:01[           ngx_epoll_process_events,  1720]  [debug] 20090#20090: epoll: fd:9 EPOLLIN  (ev:0001) d:080E36C0
2016/01/07 12:38:01[           ngx_epoll_process_events,  1766]  [debug] 20090#20090: post event 080E3680
2016/01/07 12:38:01[           ngx_event_process_posted,    65]  [debug] 20090#20090: begin to run befor posted event 080E3680
2016/01/07 12:38:01[           ngx_event_process_posted,    67]  [debug] 20090#20090: delete posted event 080E3680
2016/01/07 12:38:01[            ngx_thread_pool_handler,   401]  [debug] 20090#20090: thread pool handler
2016/01/07 12:38:01[            ngx_thread_pool_handler,   422]  [debug] 20090#20090: run completion handler for task #188
2016/01/07 12:38:01[           ngx_http_request_handler,  2401]  [debug] 20090#20090: *14 http run request(ev->write:1): "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                    ngx_http_writer,  3052]  [debug] 20090#20090: *14 http writer handler: "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[             ngx_http_output_filter,  3372]  [debug] 20090#20090: *14 http output filter "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[               ngx_http_copy_filter,   206]  [debug] 20090#20090: *14 http copy filter: "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931", r->aio:0
2016/01/07 12:38:01[                   ngx_output_chain,    67][yangya  [debug] 20090#20090: *14 ctx->sendfile:0, ctx->aio:0, ctx->directio:1
2016/01/07 12:38:01[             ngx_output_chain_as_is,   309][yangya  [debug] 20090#20090: ngx_output_chain_as_is--- buf_special:0, in_file:1, directio:1, buf_in_mem:0,need_in_memory:0, need_in_temp:0, memory:0, mmap:0
2016/01/07 12:38:01[                    ngx_thread_read,   147]  [debug] 20090#20090: *14 thread read: fd:13, buf:08115C00, size:32768, offset:163840
2016/01/07 12:38:01[             ngx_output_chain_as_is,   309][yangya  [debug] 20090#20090: ngx_output_chain_as_is--- buf_special:0, in_file:1, directio:1, buf_in_mem:0,need_in_memory:0, need_in_temp:0, memory:0, mmap:0
2016/01/07 12:38:01[           ngx_http_postpone_filter,   176]  [debug] 20090#20090: *14 http postpone filter "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931" 080F3438
2016/01/07 12:38:01[              ngx_http_write_filter,   208]  [debug] 20090#20090: *14 write new buf temporary:1 buf-in-file:0, buf->start:08115C00, buf->pos:08115C00, buf_size: 32768 file_pos: 0, in_file_size: 0
2016/01/07 12:38:01[              ngx_http_write_filter,   248]  [debug] 20090#20090: *14 http write filter: last:0 flush:1 size:32768
2016/01/07 12:38:01[              ngx_http_write_filter,   380]  [debug] 20090#20090: *14 http write filter limit 0
2016/01/07 12:38:01[           ngx_linux_sendfile_chain,   201][yangya  [debug] 20090#20090: *14 @@@@@@@@@@@@@@@@@@@@@@@begin ngx_linux_sendfile_chain @@@@@@@@@@@@@@@@@@@
2016/01/07 12:38:01[                         ngx_writev,   238]  [debug] 20090#20090: *14 writev: 32768 of 32768
2016/01/07 12:38:01[              ngx_http_write_filter,   386]  [debug] 20090#20090: *14 http write filter 00000000
2016/01/07 12:38:01[             ngx_output_chain_as_is,   309][yangya  [debug] 20090#20090: ngx_output_chain_as_is--- buf_special:0, in_file:1, directio:1, buf_in_mem:0,need_in_memory:0, need_in_temp:0, memory:0, mmap:0
2016/01/07 12:38:01[                    ngx_thread_read,   147]  [debug] 20090#20090: *14 thread read: fd:13, buf:08115C00, size:32768, offset:196608
2016/01/07 12:38:01[              ngx_thread_mutex_lock,   145]  [debug] 20090#20090: pthread_mutex_lock(080F03B0) enter
2016/01/07 12:38:01[               ngx_thread_task_post,   280][yangya  [debug] 20090#20090: ngx add task to thread, task id:189
2016/01/07 12:38:01[             ngx_thread_cond_signal,    54]  [debug] 20090#20090: pthread_cond_signal(080F03D4)
2016/01/07 12:38:01[               ngx_thread_cond_wait,    96]  [debug] 20090#20094: pthread_cond_wait(080F03D4) exit
2016/01/07 12:38:01[            ngx_thread_mutex_unlock,   171]  [debug] 20090#20094: pthread_mutex_unlock(080F03B0) exit
2016/01/07 12:38:01[              ngx_thread_pool_cycle,   370]  [debug] 20090#20094: run task #189 in thread pool name:"yang_pool"
2016/01/07 12:38:01[            ngx_thread_read_handler,   201]  [debug] 20090#20094: thread read handler
2016/01/07 12:38:01[            ngx_thread_read_handler,   219]  [debug] 20090#20094: pread: 32768 (err: 0) of 32768 @196608
2016/01/07 12:38:01[              ngx_thread_pool_cycle,   376]  [debug] 20090#20094: complete task #189 in thread pool name: "yang_pool"
2016/01/07 12:38:01[              ngx_thread_mutex_lock,   145]  [debug] 20090#20094: pthread_mutex_lock(080F03B0) enter
2016/01/07 12:38:01[               ngx_thread_cond_wait,    70]  [debug] 20090#20094: pthread_cond_wait(080F03D4) enter
2016/01/07 12:38:01[            ngx_thread_mutex_unlock,   171]  [debug] 20090#20090: pthread_mutex_unlock(080F03B0) exit
2016/01/07 12:38:01[               ngx_thread_task_post,   297]  [debug] 20090#20090: task #189 added to thread pool name: "yang_pool" complete
2016/01/07 12:38:01[               ngx_http_copy_filter,   284]  [debug] 20090#20090: *14 http copy filter rc: -2, buffered:4 "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                    ngx_http_writer,  3118]  [debug] 20090#20090: *14 http writer output filter: -2, "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                ngx_event_add_timer,    89]  [debug] 20090#20090: *14 <          ngx_http_writer,  3139>  event timer: 12, old: 442594977, new: 442595067, 
2016/01/07 12:38:01[           ngx_trylock_accept_mutex,   405]  [debug] 20090#20090: accept mutex locked
2016/01/07 12:38:01[           ngx_epoll_process_events,  1720]  [debug] 20090#20090: epoll: fd:9 EPOLLIN  (ev:0001) d:080E36C0
2016/01/07 12:38:01[           ngx_epoll_process_events,  1766]  [debug] 20090#20090: post event 080E3680
2016/01/07 12:38:01[           ngx_event_process_posted,    65]  [debug] 20090#20090: begin to run befor posted event 080E3680
2016/01/07 12:38:01[           ngx_event_process_posted,    67]  [debug] 20090#20090: delete posted event 080E3680
2016/01/07 12:38:01[            ngx_thread_pool_handler,   401]  [debug] 20090#20090: thread pool handler
2016/01/07 12:38:01[            ngx_thread_pool_handler,   422]  [debug] 20090#20090: run completion handler for task #189
2016/01/07 12:38:01[           ngx_http_request_handler,  2401]  [debug] 20090#20090: *14 http run request(ev->write:1): "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                    ngx_http_writer,  3052]  [debug] 20090#20090: *14 http writer handler: "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[             ngx_http_output_filter,  3372]  [debug] 20090#20090: *14 http output filter "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[               ngx_http_copy_filter,   206]  [debug] 20090#20090: *14 http copy filter: "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931", r->aio:0
2016/01/07 12:38:01[                   ngx_output_chain,    67][yangya  [debug] 20090#20090: *14 ctx->sendfile:0, ctx->aio:0, ctx->directio:1
2016/01/07 12:38:01[             ngx_output_chain_as_is,   309][yangya  [debug] 20090#20090: ngx_output_chain_as_is--- buf_special:0, in_file:1, directio:1, buf_in_mem:0,need_in_memory:0, need_in_temp:0, memory:0, mmap:0
2016/01/07 12:38:01[                    ngx_thread_read,   147]  [debug] 20090#20090: *14 thread read: fd:13, buf:08115C00, size:32768, offset:196608
2016/01/07 12:38:01[             ngx_output_chain_as_is,   309][yangya  [debug] 20090#20090: ngx_output_chain_as_is--- buf_special:0, in_file:1, directio:1, buf_in_mem:0,need_in_memory:0, need_in_temp:0, memory:0, mmap:0
2016/01/07 12:38:01[           ngx_http_postpone_filter,   176]  [debug] 20090#20090: *14 http postpone filter "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931" 080F3438
2016/01/07 12:38:01[              ngx_http_write_filter,   208]  [debug] 20090#20090: *14 write new buf temporary:1 buf-in-file:0, buf->start:08115C00, buf->pos:08115C00, buf_size: 32768 file_pos: 0, in_file_size: 0
2016/01/07 12:38:01[              ngx_http_write_filter,   248]  [debug] 20090#20090: *14 http write filter: last:0 flush:1 size:32768
2016/01/07 12:38:01[              ngx_http_write_filter,   380]  [debug] 20090#20090: *14 http write filter limit 0
2016/01/07 12:38:01[           ngx_linux_sendfile_chain,   201][yangya  [debug] 20090#20090: *14 @@@@@@@@@@@@@@@@@@@@@@@begin ngx_linux_sendfile_chain @@@@@@@@@@@@@@@@@@@
2016/01/07 12:38:01[                         ngx_writev,   238]  [debug] 20090#20090: *14 writev: 32768 of 32768
2016/01/07 12:38:01[              ngx_http_write_filter,   386]  [debug] 20090#20090: *14 http write filter 00000000
2016/01/07 12:38:01[             ngx_output_chain_as_is,   309][yangya  [debug] 20090#20090: ngx_output_chain_as_is--- buf_special:0, in_file:1, directio:1, buf_in_mem:0,need_in_memory:0, need_in_temp:0, memory:0, mmap:0
2016/01/07 12:38:01[                    ngx_thread_read,   147]  [debug] 20090#20090: *14 thread read: fd:13, buf:08115C00, size:32768, offset:229376
2016/01/07 12:38:01[              ngx_thread_mutex_lock,   145]  [debug] 20090#20090: pthread_mutex_lock(080F03B0) enter
2016/01/07 12:38:01[               ngx_thread_task_post,   280][yangya  [debug] 20090#20090: ngx add task to thread, task id:190
2016/01/07 12:38:01[             ngx_thread_cond_signal,    54]  [debug] 20090#20090: pthread_cond_signal(080F03D4)
2016/01/07 12:38:01[               ngx_thread_cond_wait,    96]  [debug] 20090#20094: pthread_cond_wait(080F03D4) exit
2016/01/07 12:38:01[            ngx_thread_mutex_unlock,   171]  [debug] 20090#20094: pthread_mutex_unlock(080F03B0) exit
2016/01/07 12:38:01[              ngx_thread_pool_cycle,   370]  [debug] 20090#20094: run task #190 in thread pool name:"yang_pool"
2016/01/07 12:38:01[            ngx_thread_read_handler,   201]  [debug] 20090#20094: thread read handler
2016/01/07 12:38:01[            ngx_thread_read_handler,   219]  [debug] 20090#20094: pread: 32768 (err: 0) of 32768 @229376
2016/01/07 12:38:01[              ngx_thread_pool_cycle,   376]  [debug] 20090#20094: complete task #190 in thread pool name: "yang_pool"
2016/01/07 12:38:01[              ngx_thread_mutex_lock,   145]  [debug] 20090#20094: pthread_mutex_lock(080F03B0) enter
2016/01/07 12:38:01[               ngx_thread_cond_wait,    70]  [debug] 20090#20094: pthread_cond_wait(080F03D4) enter
2016/01/07 12:38:01[            ngx_thread_mutex_unlock,   171]  [debug] 20090#20090: pthread_mutex_unlock(080F03B0) exit
2016/01/07 12:38:01[               ngx_thread_task_post,   297]  [debug] 20090#20090: task #190 added to thread pool name: "yang_pool" complete
2016/01/07 12:38:01[               ngx_http_copy_filter,   284]  [debug] 20090#20090: *14 http copy filter rc: -2, buffered:4 "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                    ngx_http_writer,  3118]  [debug] 20090#20090: *14 http writer output filter: -2, "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                ngx_event_add_timer,    89]  [debug] 20090#20090: *14 <          ngx_http_writer,  3139>  event timer: 12, old: 442594977, new: 442595072, 
2016/01/07 12:38:01[           ngx_trylock_accept_mutex,   405]  [debug] 20090#20090: accept mutex locked
2016/01/07 12:38:01[           ngx_epoll_process_events,  1720]  [debug] 20090#20090: epoll: fd:9 EPOLLIN  (ev:0001) d:080E36C0
2016/01/07 12:38:01[           ngx_epoll_process_events,  1766]  [debug] 20090#20090: post event 080E3680
2016/01/07 12:38:01[           ngx_event_process_posted,    65]  [debug] 20090#20090: begin to run befor posted event 080E3680
2016/01/07 12:38:01[           ngx_event_process_posted,    67]  [debug] 20090#20090: delete posted event 080E3680
2016/01/07 12:38:01[            ngx_thread_pool_handler,   401]  [debug] 20090#20090: thread pool handler
2016/01/07 12:38:01[            ngx_thread_pool_handler,   422]  [debug] 20090#20090: run completion handler for task #190
2016/01/07 12:38:01[           ngx_http_request_handler,  2401]  [debug] 20090#20090: *14 http run request(ev->write:1): "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                    ngx_http_writer,  3052]  [debug] 20090#20090: *14 http writer handler: "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[             ngx_http_output_filter,  3372]  [debug] 20090#20090: *14 http output filter "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[               ngx_http_copy_filter,   206]  [debug] 20090#20090: *14 http copy filter: "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931", r->aio:0
2016/01/07 12:38:01[                   ngx_output_chain,    67][yangya  [debug] 20090#20090: *14 ctx->sendfile:0, ctx->aio:0, ctx->directio:1
2016/01/07 12:38:01[             ngx_output_chain_as_is,   309][yangya  [debug] 20090#20090: ngx_output_chain_as_is--- buf_special:0, in_file:1, directio:1, buf_in_mem:0,need_in_memory:0, need_in_temp:0, memory:0, mmap:0
2016/01/07 12:38:01[                    ngx_thread_read,   147]  [debug] 20090#20090: *14 thread read: fd:13, buf:08115C00, size:32768, offset:229376
2016/01/07 12:38:01[             ngx_output_chain_as_is,   309][yangya  [debug] 20090#20090: ngx_output_chain_as_is--- buf_special:0, in_file:1, directio:1, buf_in_mem:0,need_in_memory:0, need_in_temp:0, memory:0, mmap:0
2016/01/07 12:38:01[           ngx_http_postpone_filter,   176]  [debug] 20090#20090: *14 http postpone filter "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931" 080F3438
2016/01/07 12:38:01[              ngx_http_write_filter,   208]  [debug] 20090#20090: *14 write new buf temporary:1 buf-in-file:0, buf->start:08115C00, buf->pos:08115C00, buf_size: 32768 file_pos: 0, in_file_size: 0
2016/01/07 12:38:01[              ngx_http_write_filter,   248]  [debug] 20090#20090: *14 http write filter: last:0 flush:1 size:32768
2016/01/07 12:38:01[              ngx_http_write_filter,   380]  [debug] 20090#20090: *14 http write filter limit 0
2016/01/07 12:38:01[           ngx_linux_sendfile_chain,   201][yangya  [debug] 20090#20090: *14 @@@@@@@@@@@@@@@@@@@@@@@begin ngx_linux_sendfile_chain @@@@@@@@@@@@@@@@@@@
2016/01/07 12:38:01[                         ngx_writev,   238]  [debug] 20090#20090: *14 writev: 31632 of 32768
2016/01/07 12:38:01[              ngx_http_write_filter,   386]  [debug] 20090#20090: *14 http write filter 080F3440
2016/01/07 12:38:01[             ngx_output_chain_as_is,   309][yangya  [debug] 20090#20090: ngx_output_chain_as_is--- buf_special:0, in_file:1, directio:1, buf_in_mem:0,need_in_memory:0, need_in_temp:0, memory:0, mmap:0
2016/01/07 12:38:01[               ngx_http_copy_filter,   284]  [debug] 20090#20090: *14 http copy filter rc: -2, buffered:4 "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                    ngx_http_writer,  3118]  [debug] 20090#20090: *14 http writer output filter: -2, "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                ngx_event_add_timer,    89]  [debug] 20090#20090: *14 <          ngx_http_writer,  3139>  event timer: 12, old: 442594977, new: 442595077, 
2016/01/07 12:38:01[           ngx_trylock_accept_mutex,   405]  [debug] 20090#20090: accept mutex locked
2016/01/07 12:38:01[           ngx_epoll_process_events,  1720]  [debug] 20090#20090: epoll: fd:12 EPOLLOUT  (ev:0004) d:B2593158
2016/01/07 12:38:01[           ngx_epoll_process_events,  1793]  [debug] 20090#20090: *14 post event AE993098
2016/01/07 12:38:01[           ngx_event_process_posted,    65]  [debug] 20090#20090: begin to run befor posted event AE993098
2016/01/07 12:38:01[           ngx_event_process_posted,    67]  [debug] 20090#20090: *14 delete posted event AE993098
2016/01/07 12:38:01[           ngx_http_request_handler,  2401]  [debug] 20090#20090: *14 http run request(ev->write:1): "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                    ngx_http_writer,  3052]  [debug] 20090#20090: *14 http writer handler: "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[             ngx_http_output_filter,  3372]  [debug] 20090#20090: *14 http output filter "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[               ngx_http_copy_filter,   206]  [debug] 20090#20090: *14 http copy filter: "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931", r->aio:0
2016/01/07 12:38:01[                   ngx_output_chain,    67][yangya  [debug] 20090#20090: *14 ctx->sendfile:0, ctx->aio:0, ctx->directio:1
2016/01/07 12:38:01[             ngx_output_chain_as_is,   309][yangya  [debug] 20090#20090: ngx_output_chain_as_is--- buf_special:0, in_file:1, directio:1, buf_in_mem:0,need_in_memory:0, need_in_temp:0, memory:0, mmap:0
2016/01/07 12:38:01[           ngx_http_postpone_filter,   176]  [debug] 20090#20090: *14 http postpone filter "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931" 00000000
2016/01/07 12:38:01[              ngx_http_write_filter,   151]  [debug] 20090#20090: *14 write old buf t:1 f:0 08115C00, pos 0811D790, size: 1136 file: 0, size: 0
2016/01/07 12:38:01[              ngx_http_write_filter,   248]  [debug] 20090#20090: *14 http write filter: last:0 flush:1 size:1136
2016/01/07 12:38:01[              ngx_http_write_filter,   380]  [debug] 20090#20090: *14 http write filter limit 0
2016/01/07 12:38:01[           ngx_linux_sendfile_chain,   201][yangya  [debug] 20090#20090: *14 @@@@@@@@@@@@@@@@@@@@@@@begin ngx_linux_sendfile_chain @@@@@@@@@@@@@@@@@@@
2016/01/07 12:38:01[                         ngx_writev,   238]  [debug] 20090#20090: *14 writev: 1136 of 1136
2016/01/07 12:38:01[              ngx_http_write_filter,   386]  [debug] 20090#20090: *14 http write filter 00000000
2016/01/07 12:38:01[             ngx_output_chain_as_is,   309][yangya  [debug] 20090#20090: ngx_output_chain_as_is--- buf_special:0, in_file:1, directio:1, buf_in_mem:0,need_in_memory:0, need_in_temp:0, memory:0, mmap:0
2016/01/07 12:38:01[                    ngx_thread_read,   147]  [debug] 20090#20090: *14 thread read: fd:13, buf:08115C00, size:32768, offset:262144
2016/01/07 12:38:01[              ngx_thread_mutex_lock,   145]  [debug] 20090#20090: pthread_mutex_lock(080F03B0) enter
2016/01/07 12:38:01[               ngx_thread_task_post,   280][yangya  [debug] 20090#20090: ngx add task to thread, task id:191
2016/01/07 12:38:01[             ngx_thread_cond_signal,    54]  [debug] 20090#20090: pthread_cond_signal(080F03D4)
2016/01/07 12:38:01[               ngx_thread_cond_wait,    96]  [debug] 20090#20094: pthread_cond_wait(080F03D4) exit
2016/01/07 12:38:01[            ngx_thread_mutex_unlock,   171]  [debug] 20090#20094: pthread_mutex_unlock(080F03B0) exit
2016/01/07 12:38:01[              ngx_thread_pool_cycle,   370]  [debug] 20090#20094: run task #191 in thread pool name:"yang_pool"
2016/01/07 12:38:01[            ngx_thread_read_handler,   201]  [debug] 20090#20094: thread read handler
2016/01/07 12:38:01[            ngx_thread_read_handler,   219]  [debug] 20090#20094: pread: 32768 (err: 0) of 32768 @262144
2016/01/07 12:38:01[              ngx_thread_pool_cycle,   376]  [debug] 20090#20094: complete task #191 in thread pool name: "yang_pool"
2016/01/07 12:38:01[              ngx_thread_mutex_lock,   145]  [debug] 20090#20094: pthread_mutex_lock(080F03B0) enter
2016/01/07 12:38:01[               ngx_thread_cond_wait,    70]  [debug] 20090#20094: pthread_cond_wait(080F03D4) enter
2016/01/07 12:38:01[            ngx_thread_mutex_unlock,   171]  [debug] 20090#20090: pthread_mutex_unlock(080F03B0) exit
2016/01/07 12:38:01[               ngx_thread_task_post,   297]  [debug] 20090#20090: task #191 added to thread pool name: "yang_pool" complete
2016/01/07 12:38:01[               ngx_http_copy_filter,   284]  [debug] 20090#20090: *14 http copy filter rc: -2, buffered:4 "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                    ngx_http_writer,  3118]  [debug] 20090#20090: *14 http writer output filter: -2, "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                ngx_event_add_timer,    89]  [debug] 20090#20090: *14 <          ngx_http_writer,  3139>  event timer: 12, old: 442594977, new: 442595094, 
2016/01/07 12:38:01[           ngx_trylock_accept_mutex,   405]  [debug] 20090#20090: accept mutex locked
2016/01/07 12:38:01[           ngx_epoll_process_events,  1720]  [debug] 20090#20090: epoll: fd:9 EPOLLIN  (ev:0001) d:080E36C0
2016/01/07 12:38:01[           ngx_epoll_process_events,  1766]  [debug] 20090#20090: post event 080E3680
2016/01/07 12:38:01[           ngx_event_process_posted,    65]  [debug] 20090#20090: begin to run befor posted event 080E3680
2016/01/07 12:38:01[           ngx_event_process_posted,    67]  [debug] 20090#20090: delete posted event 080E3680
2016/01/07 12:38:01[            ngx_thread_pool_handler,   401]  [debug] 20090#20090: thread pool handler
2016/01/07 12:38:01[            ngx_thread_pool_handler,   422]  [debug] 20090#20090: run completion handler for task #191
2016/01/07 12:38:01[           ngx_http_request_handler,  2401]  [debug] 20090#20090: *14 http run request(ev->write:1): "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                    ngx_http_writer,  3052]  [debug] 20090#20090: *14 http writer handler: "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[             ngx_http_output_filter,  3372]  [debug] 20090#20090: *14 http output filter "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[               ngx_http_copy_filter,   206]  [debug] 20090#20090: *14 http copy filter: "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931", r->aio:0
2016/01/07 12:38:01[                   ngx_output_chain,    67][yangya  [debug] 20090#20090: *14 ctx->sendfile:0, ctx->aio:0, ctx->directio:1
2016/01/07 12:38:01[             ngx_output_chain_as_is,   309][yangya  [debug] 20090#20090: ngx_output_chain_as_is--- buf_special:0, in_file:1, directio:1, buf_in_mem:0,need_in_memory:0, need_in_temp:0, memory:0, mmap:0
2016/01/07 12:38:01[                    ngx_thread_read,   147]  [debug] 20090#20090: *14 thread read: fd:13, buf:08115C00, size:32768, offset:262144
2016/01/07 12:38:01[             ngx_output_chain_as_is,   309][yangya  [debug] 20090#20090: ngx_output_chain_as_is--- buf_special:0, in_file:1, directio:1, buf_in_mem:0,need_in_memory:0, need_in_temp:0, memory:0, mmap:0
2016/01/07 12:38:01[           ngx_http_postpone_filter,   176]  [debug] 20090#20090: *14 http postpone filter "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931" 080F3438
2016/01/07 12:38:01[              ngx_http_write_filter,   208]  [debug] 20090#20090: *14 write new buf temporary:1 buf-in-file:0, buf->start:08115C00, buf->pos:08115C00, buf_size: 32768 file_pos: 0, in_file_size: 0
2016/01/07 12:38:01[              ngx_http_write_filter,   248]  [debug] 20090#20090: *14 http write filter: last:0 flush:1 size:32768
2016/01/07 12:38:01[              ngx_http_write_filter,   380]  [debug] 20090#20090: *14 http write filter limit 0
2016/01/07 12:38:01[           ngx_linux_sendfile_chain,   201][yangya  [debug] 20090#20090: *14 @@@@@@@@@@@@@@@@@@@@@@@begin ngx_linux_sendfile_chain @@@@@@@@@@@@@@@@@@@
2016/01/07 12:38:01[                         ngx_writev,   238]  [debug] 20090#20090: *14 writev: 32768 of 32768
2016/01/07 12:38:01[              ngx_http_write_filter,   386]  [debug] 20090#20090: *14 http write filter 00000000
2016/01/07 12:38:01[             ngx_output_chain_as_is,   309][yangya  [debug] 20090#20090: ngx_output_chain_as_is--- buf_special:0, in_file:1, directio:1, buf_in_mem:0,need_in_memory:0, need_in_temp:0, memory:0, mmap:0
2016/01/07 12:38:01[                    ngx_thread_read,   147]  [debug] 20090#20090: *14 thread read: fd:13, buf:08115C00, size:32768, offset:294912
2016/01/07 12:38:01[              ngx_thread_mutex_lock,   145]  [debug] 20090#20090: pthread_mutex_lock(080F03B0) enter
2016/01/07 12:38:01[               ngx_thread_task_post,   280][yangya  [debug] 20090#20090: ngx add task to thread, task id:192
2016/01/07 12:38:01[             ngx_thread_cond_signal,    54]  [debug] 20090#20090: pthread_cond_signal(080F03D4)
2016/01/07 12:38:01[               ngx_thread_cond_wait,    96]  [debug] 20090#20094: pthread_cond_wait(080F03D4) exit
2016/01/07 12:38:01[            ngx_thread_mutex_unlock,   171]  [debug] 20090#20094: pthread_mutex_unlock(080F03B0) exit
2016/01/07 12:38:01[              ngx_thread_pool_cycle,   370]  [debug] 20090#20094: run task #192 in thread pool name:"yang_pool"
2016/01/07 12:38:01[            ngx_thread_read_handler,   201]  [debug] 20090#20094: thread read handler
2016/01/07 12:38:01[            ngx_thread_read_handler,   219]  [debug] 20090#20094: pread: 32768 (err: 0) of 32768 @294912
2016/01/07 12:38:01[              ngx_thread_pool_cycle,   376]  [debug] 20090#20094: complete task #192 in thread pool name: "yang_pool"
2016/01/07 12:38:01[              ngx_thread_mutex_lock,   145]  [debug] 20090#20094: pthread_mutex_lock(080F03B0) enter
2016/01/07 12:38:01[               ngx_thread_cond_wait,    70]  [debug] 20090#20094: pthread_cond_wait(080F03D4) enter
2016/01/07 12:38:01[            ngx_thread_mutex_unlock,   171]  [debug] 20090#20090: pthread_mutex_unlock(080F03B0) exit
2016/01/07 12:38:01[               ngx_thread_task_post,   297]  [debug] 20090#20090: task #192 added to thread pool name: "yang_pool" complete
2016/01/07 12:38:01[               ngx_http_copy_filter,   284]  [debug] 20090#20090: *14 http copy filter rc: -2, buffered:4 "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                    ngx_http_writer,  3118]  [debug] 20090#20090: *14 http writer output filter: -2, "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                ngx_event_add_timer,    89]  [debug] 20090#20090: *14 <          ngx_http_writer,  3139>  event timer: 12, old: 442594977, new: 442595095, 
2016/01/07 12:38:01[           ngx_trylock_accept_mutex,   405]  [debug] 20090#20090: accept mutex locked
2016/01/07 12:38:01[           ngx_epoll_process_events,  1720]  [debug] 20090#20090: epoll: fd:9 EPOLLIN  (ev:0001) d:080E36C0
2016/01/07 12:38:01[           ngx_epoll_process_events,  1766]  [debug] 20090#20090: post event 080E3680
2016/01/07 12:38:01[           ngx_event_process_posted,    65]  [debug] 20090#20090: begin to run befor posted event 080E3680
2016/01/07 12:38:01[           ngx_event_process_posted,    67]  [debug] 20090#20090: delete posted event 080E3680
2016/01/07 12:38:01[            ngx_thread_pool_handler,   401]  [debug] 20090#20090: thread pool handler
2016/01/07 12:38:01[            ngx_thread_pool_handler,   422]  [debug] 20090#20090: run completion handler for task #192
2016/01/07 12:38:01[           ngx_http_request_handler,  2401]  [debug] 20090#20090: *14 http run request(ev->write:1): "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                    ngx_http_writer,  3052]  [debug] 20090#20090: *14 http writer handler: "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[             ngx_http_output_filter,  3372]  [debug] 20090#20090: *14 http output filter "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[               ngx_http_copy_filter,   206]  [debug] 20090#20090: *14 http copy filter: "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931", r->aio:0
2016/01/07 12:38:01[                   ngx_output_chain,    67][yangya  [debug] 20090#20090: *14 ctx->sendfile:0, ctx->aio:0, ctx->directio:1
2016/01/07 12:38:01[             ngx_output_chain_as_is,   309][yangya  [debug] 20090#20090: ngx_output_chain_as_is--- buf_special:0, in_file:1, directio:1, buf_in_mem:0,need_in_memory:0, need_in_temp:0, memory:0, mmap:0
2016/01/07 12:38:01[                    ngx_thread_read,   147]  [debug] 20090#20090: *14 thread read: fd:13, buf:08115C00, size:32768, offset:294912
2016/01/07 12:38:01[             ngx_output_chain_as_is,   309][yangya  [debug] 20090#20090: ngx_output_chain_as_is--- buf_special:0, in_file:1, directio:1, buf_in_mem:0,need_in_memory:0, need_in_temp:0, memory:0, mmap:0
2016/01/07 12:38:01[           ngx_http_postpone_filter,   176]  [debug] 20090#20090: *14 http postpone filter "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931" 080F3438
2016/01/07 12:38:01[              ngx_http_write_filter,   208]  [debug] 20090#20090: *14 write new buf temporary:1 buf-in-file:0, buf->start:08115C00, buf->pos:08115C00, buf_size: 32768 file_pos: 0, in_file_size: 0
2016/01/07 12:38:01[              ngx_http_write_filter,   248]  [debug] 20090#20090: *14 http write filter: last:0 flush:1 size:32768
2016/01/07 12:38:01[              ngx_http_write_filter,   380]  [debug] 20090#20090: *14 http write filter limit 0
2016/01/07 12:38:01[           ngx_linux_sendfile_chain,   201][yangya  [debug] 20090#20090: *14 @@@@@@@@@@@@@@@@@@@@@@@begin ngx_linux_sendfile_chain @@@@@@@@@@@@@@@@@@@
2016/01/07 12:38:01[                         ngx_writev,   238]  [debug] 20090#20090: *14 writev: 32768 of 32768
2016/01/07 12:38:01[              ngx_http_write_filter,   386]  [debug] 20090#20090: *14 http write filter 00000000
2016/01/07 12:38:01[             ngx_output_chain_as_is,   309][yangya  [debug] 20090#20090: ngx_output_chain_as_is--- buf_special:0, in_file:1, directio:1, buf_in_mem:0,need_in_memory:0, need_in_temp:0, memory:0, mmap:0
2016/01/07 12:38:01[                    ngx_thread_read,   147]  [debug] 20090#20090: *14 thread read: fd:13, buf:08115C00, size:32768, offset:327680
2016/01/07 12:38:01[              ngx_thread_mutex_lock,   145]  [debug] 20090#20090: pthread_mutex_lock(080F03B0) enter
2016/01/07 12:38:01[               ngx_thread_task_post,   280][yangya  [debug] 20090#20090: ngx add task to thread, task id:193
2016/01/07 12:38:01[             ngx_thread_cond_signal,    54]  [debug] 20090#20090: pthread_cond_signal(080F03D4)
2016/01/07 12:38:01[               ngx_thread_cond_wait,    96]  [debug] 20090#20094: pthread_cond_wait(080F03D4) exit
2016/01/07 12:38:01[            ngx_thread_mutex_unlock,   171]  [debug] 20090#20094: pthread_mutex_unlock(080F03B0) exit
2016/01/07 12:38:01[              ngx_thread_pool_cycle,   370]  [debug] 20090#20094: run task #193 in thread pool name:"yang_pool"
2016/01/07 12:38:01[            ngx_thread_read_handler,   201]  [debug] 20090#20094: thread read handler
2016/01/07 12:38:01[            ngx_thread_read_handler,   219]  [debug] 20090#20094: pread: 32768 (err: 0) of 32768 @327680
2016/01/07 12:38:01[              ngx_thread_pool_cycle,   376]  [debug] 20090#20094: complete task #193 in thread pool name: "yang_pool"
2016/01/07 12:38:01[              ngx_thread_mutex_lock,   145]  [debug] 20090#20094: pthread_mutex_lock(080F03B0) enter
2016/01/07 12:38:01[               ngx_thread_cond_wait,    70]  [debug] 20090#20094: pthread_cond_wait(080F03D4) enter
2016/01/07 12:38:01[            ngx_thread_mutex_unlock,   171]  [debug] 20090#20090: pthread_mutex_unlock(080F03B0) exit
2016/01/07 12:38:01[               ngx_thread_task_post,   297]  [debug] 20090#20090: task #193 added to thread pool name: "yang_pool" complete
2016/01/07 12:38:01[               ngx_http_copy_filter,   284]  [debug] 20090#20090: *14 http copy filter rc: -2, buffered:4 "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                    ngx_http_writer,  3118]  [debug] 20090#20090: *14 http writer output filter: -2, "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                ngx_event_add_timer,    89]  [debug] 20090#20090: *14 <          ngx_http_writer,  3139>  event timer: 12, old: 442594977, new: 442595097, 
2016/01/07 12:38:01[           ngx_trylock_accept_mutex,   405]  [debug] 20090#20090: accept mutex locked
2016/01/07 12:38:01[           ngx_epoll_process_events,  1720]  [debug] 20090#20090: epoll: fd:9 EPOLLIN  (ev:0001) d:080E36C0
2016/01/07 12:38:01[           ngx_epoll_process_events,  1766]  [debug] 20090#20090: post event 080E3680
2016/01/07 12:38:01[           ngx_event_process_posted,    65]  [debug] 20090#20090: begin to run befor posted event 080E3680
2016/01/07 12:38:01[           ngx_event_process_posted,    67]  [debug] 20090#20090: delete posted event 080E3680
2016/01/07 12:38:01[            ngx_thread_pool_handler,   401]  [debug] 20090#20090: thread pool handler
2016/01/07 12:38:01[            ngx_thread_pool_handler,   422]  [debug] 20090#20090: run completion handler for task #193
2016/01/07 12:38:01[           ngx_http_request_handler,  2401]  [debug] 20090#20090: *14 http run request(ev->write:1): "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                    ngx_http_writer,  3052]  [debug] 20090#20090: *14 http writer handler: "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[             ngx_http_output_filter,  3372]  [debug] 20090#20090: *14 http output filter "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[               ngx_http_copy_filter,   206]  [debug] 20090#20090: *14 http copy filter: "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931", r->aio:0
2016/01/07 12:38:01[                   ngx_output_chain,    67][yangya  [debug] 20090#20090: *14 ctx->sendfile:0, ctx->aio:0, ctx->directio:1
2016/01/07 12:38:01[             ngx_output_chain_as_is,   309][yangya  [debug] 20090#20090: ngx_output_chain_as_is--- buf_special:0, in_file:1, directio:1, buf_in_mem:0,need_in_memory:0, need_in_temp:0, memory:0, mmap:0
2016/01/07 12:38:01[                    ngx_thread_read,   147]  [debug] 20090#20090: *14 thread read: fd:13, buf:08115C00, size:32768, offset:327680
2016/01/07 12:38:01[             ngx_output_chain_as_is,   309][yangya  [debug] 20090#20090: ngx_output_chain_as_is--- buf_special:0, in_file:1, directio:1, buf_in_mem:0,need_in_memory:0, need_in_temp:0, memory:0, mmap:0
2016/01/07 12:38:01[           ngx_http_postpone_filter,   176]  [debug] 20090#20090: *14 http postpone filter "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931" 080F3438
2016/01/07 12:38:01[              ngx_http_write_filter,   208]  [debug] 20090#20090: *14 write new buf temporary:1 buf-in-file:0, buf->start:08115C00, buf->pos:08115C00, buf_size: 32768 file_pos: 0, in_file_size: 0
2016/01/07 12:38:01[              ngx_http_write_filter,   248]  [debug] 20090#20090: *14 http write filter: last:0 flush:1 size:32768
2016/01/07 12:38:01[              ngx_http_write_filter,   380]  [debug] 20090#20090: *14 http write filter limit 0
2016/01/07 12:38:01[           ngx_linux_sendfile_chain,   201][yangya  [debug] 20090#20090: *14 @@@@@@@@@@@@@@@@@@@@@@@begin ngx_linux_sendfile_chain @@@@@@@@@@@@@@@@@@@
2016/01/07 12:38:01[                         ngx_writev,   238]  [debug] 20090#20090: *14 writev: 32768 of 32768
2016/01/07 12:38:01[              ngx_http_write_filter,   386]  [debug] 20090#20090: *14 http write filter 00000000
2016/01/07 12:38:01[             ngx_output_chain_as_is,   309][yangya  [debug] 20090#20090: ngx_output_chain_as_is--- buf_special:0, in_file:1, directio:1, buf_in_mem:0,need_in_memory:0, need_in_temp:0, memory:0, mmap:0
2016/01/07 12:38:01[                    ngx_thread_read,   147]  [debug] 20090#20090: *14 thread read: fd:13, buf:08115C00, size:32768, offset:360448
2016/01/07 12:38:01[              ngx_thread_mutex_lock,   145]  [debug] 20090#20090: pthread_mutex_lock(080F03B0) enter
2016/01/07 12:38:01[               ngx_thread_task_post,   280][yangya  [debug] 20090#20090: ngx add task to thread, task id:194
2016/01/07 12:38:01[             ngx_thread_cond_signal,    54]  [debug] 20090#20090: pthread_cond_signal(080F03D4)
2016/01/07 12:38:01[               ngx_thread_cond_wait,    96]  [debug] 20090#20094: pthread_cond_wait(080F03D4) exit
2016/01/07 12:38:01[            ngx_thread_mutex_unlock,   171]  [debug] 20090#20094: pthread_mutex_unlock(080F03B0) exit
2016/01/07 12:38:01[              ngx_thread_pool_cycle,   370]  [debug] 20090#20094: run task #194 in thread pool name:"yang_pool"
2016/01/07 12:38:01[            ngx_thread_read_handler,   201]  [debug] 20090#20094: thread read handler
2016/01/07 12:38:01[            ngx_thread_read_handler,   219]  [debug] 20090#20094: pread: 32768 (err: 0) of 32768 @360448
2016/01/07 12:38:01[              ngx_thread_pool_cycle,   376]  [debug] 20090#20094: complete task #194 in thread pool name: "yang_pool"
2016/01/07 12:38:01[              ngx_thread_mutex_lock,   145]  [debug] 20090#20094: pthread_mutex_lock(080F03B0) enter
2016/01/07 12:38:01[               ngx_thread_cond_wait,    70]  [debug] 20090#20094: pthread_cond_wait(080F03D4) enter
2016/01/07 12:38:01[            ngx_thread_mutex_unlock,   171]  [debug] 20090#20090: pthread_mutex_unlock(080F03B0) exit
2016/01/07 12:38:01[               ngx_thread_task_post,   297]  [debug] 20090#20090: task #194 added to thread pool name: "yang_pool" complete
2016/01/07 12:38:01[               ngx_http_copy_filter,   284]  [debug] 20090#20090: *14 http copy filter rc: -2, buffered:4 "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                    ngx_http_writer,  3118]  [debug] 20090#20090: *14 http writer output filter: -2, "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                ngx_event_add_timer,    89]  [debug] 20090#20090: *14 <          ngx_http_writer,  3139>  event timer: 12, old: 442594977, new: 442595098, 
2016/01/07 12:38:01[           ngx_trylock_accept_mutex,   405]  [debug] 20090#20090: accept mutex locked
2016/01/07 12:38:01[           ngx_epoll_process_events,  1720]  [debug] 20090#20090: epoll: fd:9 EPOLLIN  (ev:0001) d:080E36C0
2016/01/07 12:38:01[           ngx_epoll_process_events,  1766]  [debug] 20090#20090: post event 080E3680
2016/01/07 12:38:01[           ngx_event_process_posted,    65]  [debug] 20090#20090: begin to run befor posted event 080E3680
2016/01/07 12:38:01[           ngx_event_process_posted,    67]  [debug] 20090#20090: delete posted event 080E3680
2016/01/07 12:38:01[            ngx_thread_pool_handler,   401]  [debug] 20090#20090: thread pool handler
2016/01/07 12:38:01[            ngx_thread_pool_handler,   422]  [debug] 20090#20090: run completion handler for task #194
2016/01/07 12:38:01[           ngx_http_request_handler,  2401]  [debug] 20090#20090: *14 http run request(ev->write:1): "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                    ngx_http_writer,  3052]  [debug] 20090#20090: *14 http writer handler: "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[             ngx_http_output_filter,  3372]  [debug] 20090#20090: *14 http output filter "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[               ngx_http_copy_filter,   206]  [debug] 20090#20090: *14 http copy filter: "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931", r->aio:0
2016/01/07 12:38:01[                   ngx_output_chain,    67][yangya  [debug] 20090#20090: *14 ctx->sendfile:0, ctx->aio:0, ctx->directio:1
2016/01/07 12:38:01[             ngx_output_chain_as_is,   309][yangya  [debug] 20090#20090: ngx_output_chain_as_is--- buf_special:0, in_file:1, directio:1, buf_in_mem:0,need_in_memory:0, need_in_temp:0, memory:0, mmap:0
2016/01/07 12:38:01[                    ngx_thread_read,   147]  [debug] 20090#20090: *14 thread read: fd:13, buf:08115C00, size:32768, offset:360448
2016/01/07 12:38:01[             ngx_output_chain_as_is,   309][yangya  [debug] 20090#20090: ngx_output_chain_as_is--- buf_special:0, in_file:1, directio:1, buf_in_mem:0,need_in_memory:0, need_in_temp:0, memory:0, mmap:0
2016/01/07 12:38:01[           ngx_http_postpone_filter,   176]  [debug] 20090#20090: *14 http postpone filter "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931" 080F3438
2016/01/07 12:38:01[              ngx_http_write_filter,   208]  [debug] 20090#20090: *14 write new buf temporary:1 buf-in-file:0, buf->start:08115C00, buf->pos:08115C00, buf_size: 32768 file_pos: 0, in_file_size: 0
2016/01/07 12:38:01[              ngx_http_write_filter,   248]  [debug] 20090#20090: *14 http write filter: last:0 flush:1 size:32768
2016/01/07 12:38:01[              ngx_http_write_filter,   380]  [debug] 20090#20090: *14 http write filter limit 0
2016/01/07 12:38:01[           ngx_linux_sendfile_chain,   201][yangya  [debug] 20090#20090: *14 @@@@@@@@@@@@@@@@@@@@@@@begin ngx_linux_sendfile_chain @@@@@@@@@@@@@@@@@@@


















































2016/01/07 12:38:01[           ngx_linux_sendfile_chain,   201][yangya  [debug] 20090#20090: *14 @@@@@@@@@@@@@@@@@@@@@@@begin ngx_linux_sendfile_chain @@@@@@@@@@@@@@@@@@@
2016/01/07 12:38:01[                         ngx_writev,   238]  [debug] 20090#20090: *14 writev: 32768 of 32768
2016/01/07 12:38:01[              ngx_http_write_filter,   386]  [debug] 20090#20090: *14 http write filter 00000000
2016/01/07 12:38:01[             ngx_output_chain_as_is,   309][yangya  [debug] 20090#20090: ngx_output_chain_as_is--- buf_special:0, in_file:1, directio:1, buf_in_mem:0,need_in_memory:0, need_in_temp:0, memory:0, mmap:0
2016/01/07 12:38:01[                    ngx_thread_read,   147]  [debug] 20090#20090: *14 thread read: fd:13, buf:08115C00, size:32768, offset:5046272
2016/01/07 12:38:01[              ngx_thread_mutex_lock,   145]  [debug] 20090#20090: pthread_mutex_lock(080F03B0) enter
2016/01/07 12:38:01[               ngx_thread_task_post,   280][yangya  [debug] 20090#20090: ngx add task to thread, task id:337
2016/01/07 12:38:01[             ngx_thread_cond_signal,    54]  [debug] 20090#20090: pthread_cond_signal(080F03D4)
2016/01/07 12:38:01[               ngx_thread_cond_wait,    96]  [debug] 20090#20094: pthread_cond_wait(080F03D4) exit
2016/01/07 12:38:01[            ngx_thread_mutex_unlock,   171]  [debug] 20090#20094: pthread_mutex_unlock(080F03B0) exit
2016/01/07 12:38:01[              ngx_thread_pool_cycle,   370]  [debug] 20090#20094: run task #337 in thread pool name:"yang_pool"
2016/01/07 12:38:01[            ngx_thread_read_handler,   201]  [debug] 20090#20094: thread read handler
2016/01/07 12:38:01[            ngx_thread_read_handler,   219]  [debug] 20090#20094: pread: 32768 (err: 0) of 32768 @5046272
2016/01/07 12:38:01[              ngx_thread_pool_cycle,   376]  [debug] 20090#20094: complete task #337 in thread pool name: "yang_pool"
2016/01/07 12:38:01[              ngx_thread_mutex_lock,   145]  [debug] 20090#20094: pthread_mutex_lock(080F03B0) enter
2016/01/07 12:38:01[               ngx_thread_cond_wait,    70]  [debug] 20090#20094: pthread_cond_wait(080F03D4) enter
2016/01/07 12:38:01[            ngx_thread_mutex_unlock,   171]  [debug] 20090#20090: pthread_mutex_unlock(080F03B0) exit
2016/01/07 12:38:01[               ngx_thread_task_post,   297]  [debug] 20090#20090: task #337 added to thread pool name: "yang_pool" complete
2016/01/07 12:38:01[               ngx_http_copy_filter,   284]  [debug] 20090#20090: *14 http copy filter rc: -2, buffered:4 "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                    ngx_http_writer,  3118]  [debug] 20090#20090: *14 http writer output filter: -2, "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                ngx_event_add_timer,    89]  [debug] 20090#20090: *14 <          ngx_http_writer,  3139>  event timer: 12, old: 442595277, new: 442595449, 
2016/01/07 12:38:01[           ngx_trylock_accept_mutex,   405]  [debug] 20090#20090: accept mutex locked
2016/01/07 12:38:01[           ngx_epoll_process_events,  1720]  [debug] 20090#20090: epoll: fd:9 EPOLLIN  (ev:0001) d:080E36C0
2016/01/07 12:38:01[           ngx_epoll_process_events,  1766]  [debug] 20090#20090: post event 080E3680
2016/01/07 12:38:01[           ngx_event_process_posted,    65]  [debug] 20090#20090: begin to run befor posted event 080E3680
2016/01/07 12:38:01[           ngx_event_process_posted,    67]  [debug] 20090#20090: delete posted event 080E3680
2016/01/07 12:38:01[            ngx_thread_pool_handler,   401]  [debug] 20090#20090: thread pool handler
2016/01/07 12:38:01[            ngx_thread_pool_handler,   422]  [debug] 20090#20090: run completion handler for task #337
2016/01/07 12:38:01[           ngx_http_request_handler,  2401]  [debug] 20090#20090: *14 http run request(ev->write:1): "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                    ngx_http_writer,  3052]  [debug] 20090#20090: *14 http writer handler: "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[             ngx_http_output_filter,  3372]  [debug] 20090#20090: *14 http output filter "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[               ngx_http_copy_filter,   206]  [debug] 20090#20090: *14 http copy filter: "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931", r->aio:0
2016/01/07 12:38:01[                   ngx_output_chain,    67][yangya  [debug] 20090#20090: *14 ctx->sendfile:0, ctx->aio:0, ctx->directio:1
2016/01/07 12:38:01[             ngx_output_chain_as_is,   309][yangya  [debug] 20090#20090: ngx_output_chain_as_is--- buf_special:0, in_file:1, directio:1, buf_in_mem:0,need_in_memory:0, need_in_temp:0, memory:0, mmap:0
2016/01/07 12:38:01[                    ngx_thread_read,   147]  [debug] 20090#20090: *14 thread read: fd:13, buf:08115C00, size:32768, offset:5046272
2016/01/07 12:38:01[             ngx_output_chain_as_is,   309][yangya  [debug] 20090#20090: ngx_output_chain_as_is--- buf_special:0, in_file:1, directio:1, buf_in_mem:0,need_in_memory:0, need_in_temp:0, memory:0, mmap:0
2016/01/07 12:38:01[           ngx_http_postpone_filter,   176]  [debug] 20090#20090: *14 http postpone filter "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931" 080F3438
2016/01/07 12:38:01[              ngx_http_write_filter,   208]  [debug] 20090#20090: *14 write new buf temporary:1 buf-in-file:0, buf->start:08115C00, buf->pos:08115C00, buf_size: 32768 file_pos: 0, in_file_size: 0
2016/01/07 12:38:01[              ngx_http_write_filter,   248]  [debug] 20090#20090: *14 http write filter: last:0 flush:1 size:32768
2016/01/07 12:38:01[              ngx_http_write_filter,   380]  [debug] 20090#20090: *14 http write filter limit 0
2016/01/07 12:38:01[           ngx_linux_sendfile_chain,   201][yangya  [debug] 20090#20090: *14 @@@@@@@@@@@@@@@@@@@@@@@begin ngx_linux_sendfile_chain @@@@@@@@@@@@@@@@@@@
2016/01/07 12:38:01[                         ngx_writev,   238]  [debug] 20090#20090: *14 writev: 32768 of 32768
2016/01/07 12:38:01[              ngx_http_write_filter,   386]  [debug] 20090#20090: *14 http write filter 00000000
2016/01/07 12:38:01[             ngx_output_chain_as_is,   309][yangya  [debug] 20090#20090: ngx_output_chain_as_is--- buf_special:0, in_file:1, directio:1, buf_in_mem:0,need_in_memory:0, need_in_temp:0, memory:0, mmap:0
2016/01/07 12:38:01[                    ngx_thread_read,   147]  [debug] 20090#20090: *14 thread read: fd:13, buf:08115C00, size:32768, offset:5079040
2016/01/07 12:38:01[              ngx_thread_mutex_lock,   145]  [debug] 20090#20090: pthread_mutex_lock(080F03B0) enter
2016/01/07 12:38:01[               ngx_thread_task_post,   280][yangya  [debug] 20090#20090: ngx add task to thread, task id:338
2016/01/07 12:38:01[             ngx_thread_cond_signal,    54]  [debug] 20090#20090: pthread_cond_signal(080F03D4)
2016/01/07 12:38:01[               ngx_thread_cond_wait,    96]  [debug] 20090#20094: pthread_cond_wait(080F03D4) exit
2016/01/07 12:38:01[            ngx_thread_mutex_unlock,   171]  [debug] 20090#20094: pthread_mutex_unlock(080F03B0) exit
2016/01/07 12:38:01[              ngx_thread_pool_cycle,   370]  [debug] 20090#20094: run task #338 in thread pool name:"yang_pool"
2016/01/07 12:38:01[            ngx_thread_read_handler,   201]  [debug] 20090#20094: thread read handler
2016/01/07 12:38:01[            ngx_thread_read_handler,   219]  [debug] 20090#20094: pread: 32768 (err: 0) of 32768 @5079040
2016/01/07 12:38:01[              ngx_thread_pool_cycle,   376]  [debug] 20090#20094: complete task #338 in thread pool name: "yang_pool"
2016/01/07 12:38:01[              ngx_thread_mutex_lock,   145]  [debug] 20090#20094: pthread_mutex_lock(080F03B0) enter
2016/01/07 12:38:01[               ngx_thread_cond_wait,    70]  [debug] 20090#20094: pthread_cond_wait(080F03D4) enter
2016/01/07 12:38:01[            ngx_thread_mutex_unlock,   171]  [debug] 20090#20090: pthread_mutex_unlock(080F03B0) exit
2016/01/07 12:38:01[               ngx_thread_task_post,   297]  [debug] 20090#20090: task #338 added to thread pool name: "yang_pool" complete
2016/01/07 12:38:01[               ngx_http_copy_filter,   284]  [debug] 20090#20090: *14 http copy filter rc: -2, buffered:4 "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                    ngx_http_writer,  3118]  [debug] 20090#20090: *14 http writer output filter: -2, "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                ngx_event_add_timer,    89]  [debug] 20090#20090: *14 <          ngx_http_writer,  3139>  event timer: 12, old: 442595277, new: 442595450, 
2016/01/07 12:38:01[           ngx_trylock_accept_mutex,   405]  [debug] 20090#20090: accept mutex locked
2016/01/07 12:38:01[           ngx_epoll_process_events,  1720]  [debug] 20090#20090: epoll: fd:9 EPOLLIN  (ev:0001) d:080E36C0
2016/01/07 12:38:01[           ngx_epoll_process_events,  1766]  [debug] 20090#20090: post event 080E3680
2016/01/07 12:38:01[           ngx_event_process_posted,    65]  [debug] 20090#20090: begin to run befor posted event 080E3680
2016/01/07 12:38:01[           ngx_event_process_posted,    67]  [debug] 20090#20090: delete posted event 080E3680
2016/01/07 12:38:01[            ngx_thread_pool_handler,   401]  [debug] 20090#20090: thread pool handler
2016/01/07 12:38:01[            ngx_thread_pool_handler,   422]  [debug] 20090#20090: run completion handler for task #338
2016/01/07 12:38:01[           ngx_http_request_handler,  2401]  [debug] 20090#20090: *14 http run request(ev->write:1): "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                    ngx_http_writer,  3052]  [debug] 20090#20090: *14 http writer handler: "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[             ngx_http_output_filter,  3372]  [debug] 20090#20090: *14 http output filter "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[               ngx_http_copy_filter,   206]  [debug] 20090#20090: *14 http copy filter: "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931", r->aio:0
2016/01/07 12:38:01[                   ngx_output_chain,    67][yangya  [debug] 20090#20090: *14 ctx->sendfile:0, ctx->aio:0, ctx->directio:1
2016/01/07 12:38:01[             ngx_output_chain_as_is,   309][yangya  [debug] 20090#20090: ngx_output_chain_as_is--- buf_special:0, in_file:1, directio:1, buf_in_mem:0,need_in_memory:0, need_in_temp:0, memory:0, mmap:0
2016/01/07 12:38:01[                    ngx_thread_read,   147]  [debug] 20090#20090: *14 thread read: fd:13, buf:08115C00, size:32768, offset:5079040
2016/01/07 12:38:01[             ngx_output_chain_as_is,   309][yangya  [debug] 20090#20090: ngx_output_chain_as_is--- buf_special:0, in_file:1, directio:1, buf_in_mem:0,need_in_memory:0, need_in_temp:0, memory:0, mmap:0
2016/01/07 12:38:01[                    ngx_thread_read,   147]  [debug] 20090#20090: *14 thread read: fd:13, buf:080F3890, size:2340, offset:5111808
2016/01/07 12:38:01[              ngx_thread_mutex_lock,   145]  [debug] 20090#20090: pthread_mutex_lock(080F03B0) enter
2016/01/07 12:38:01[               ngx_thread_task_post,   280][yangya  [debug] 20090#20090: ngx add task to thread, task id:339
2016/01/07 12:38:01[             ngx_thread_cond_signal,    54]  [debug] 20090#20090: pthread_cond_signal(080F03D4)
2016/01/07 12:38:01[               ngx_thread_cond_wait,    96]  [debug] 20090#20094: pthread_cond_wait(080F03D4) exit
2016/01/07 12:38:01[            ngx_thread_mutex_unlock,   171]  [debug] 20090#20094: pthread_mutex_unlock(080F03B0) exit
2016/01/07 12:38:01[              ngx_thread_pool_cycle,   370]  [debug] 20090#20094: run task #339 in thread pool name:"yang_pool"
2016/01/07 12:38:01[            ngx_thread_read_handler,   201]  [debug] 20090#20094: thread read handler
2016/01/07 12:38:01[            ngx_thread_read_handler,   219]  [debug] 20090#20094: pread: 2340 (err: 0) of 2340 @5111808
2016/01/07 12:38:01[              ngx_thread_pool_cycle,   376]  [debug] 20090#20094: complete task #339 in thread pool name: "yang_pool"
2016/01/07 12:38:01[              ngx_thread_mutex_lock,   145]  [debug] 20090#20094: pthread_mutex_lock(080F03B0) enter
2016/01/07 12:38:01[               ngx_thread_cond_wait,    70]  [debug] 20090#20094: pthread_cond_wait(080F03D4) enter
2016/01/07 12:38:01[            ngx_thread_mutex_unlock,   171]  [debug] 20090#20090: pthread_mutex_unlock(080F03B0) exit
2016/01/07 12:38:01[               ngx_thread_task_post,   297]  [debug] 20090#20090: task #339 added to thread pool name: "yang_pool" complete
2016/01/07 12:38:01[           ngx_http_postpone_filter,   176]  [debug] 20090#20090: *14 http postpone filter "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931" 080F3438
2016/01/07 12:38:01[              ngx_http_write_filter,   208]  [debug] 20090#20090: *14 write new buf temporary:1 buf-in-file:0, buf->start:08115C00, buf->pos:08115C00, buf_size: 32768 file_pos: 0, in_file_size: 0
2016/01/07 12:38:01[              ngx_http_write_filter,   248]  [debug] 20090#20090: *14 http write filter: last:0 flush:1 size:32768
2016/01/07 12:38:01[              ngx_http_write_filter,   380]  [debug] 20090#20090: *14 http write filter limit 0
2016/01/07 12:38:01[           ngx_linux_sendfile_chain,   201][yangya  [debug] 20090#20090: *14 @@@@@@@@@@@@@@@@@@@@@@@begin ngx_linux_sendfile_chain @@@@@@@@@@@@@@@@@@@
2016/01/07 12:38:01[                         ngx_writev,   238]  [debug] 20090#20090: *14 writev: 32768 of 32768
2016/01/07 12:38:01[              ngx_http_write_filter,   386]  [debug] 20090#20090: *14 http write filter 00000000
2016/01/07 12:38:01[                   ngx_output_chain,   117][yangya  [debug] 20090#20090: *14 ctx->aio = 1, wait kernel complete read
2016/01/07 12:38:01[               ngx_http_copy_filter,   284]  [debug] 20090#20090: *14 http copy filter rc: -2, buffered:4 "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                    ngx_http_writer,  3118]  [debug] 20090#20090: *14 http writer output filter: -2, "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                ngx_event_add_timer,    89]  [debug] 20090#20090: *14 <          ngx_http_writer,  3139>  event timer: 12, old: 442595277, new: 442595451, 
2016/01/07 12:38:01[           ngx_trylock_accept_mutex,   405]  [debug] 20090#20090: accept mutex locked
2016/01/07 12:38:01[           ngx_epoll_process_events,  1720]  [debug] 20090#20090: epoll: fd:9 EPOLLIN  (ev:0001) d:080E36C0
2016/01/07 12:38:01[           ngx_epoll_process_events,  1766]  [debug] 20090#20090: post event 080E3680
2016/01/07 12:38:01[           ngx_event_process_posted,    65]  [debug] 20090#20090: begin to run befor posted event 080E3680
2016/01/07 12:38:01[           ngx_event_process_posted,    67]  [debug] 20090#20090: delete posted event 080E3680
2016/01/07 12:38:01[            ngx_thread_pool_handler,   401]  [debug] 20090#20090: thread pool handler
2016/01/07 12:38:01[            ngx_thread_pool_handler,   422]  [debug] 20090#20090: run completion handler for task #339
2016/01/07 12:38:01[           ngx_http_request_handler,  2401]  [debug] 20090#20090: *14 http run request(ev->write:1): "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                    ngx_http_writer,  3052]  [debug] 20090#20090: *14 http writer handler: "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[             ngx_http_output_filter,  3372]  [debug] 20090#20090: *14 http output filter "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[               ngx_http_copy_filter,   206]  [debug] 20090#20090: *14 http copy filter: "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931", r->aio:0
2016/01/07 12:38:01[                   ngx_output_chain,    67][yangya  [debug] 20090#20090: *14 ctx->sendfile:0, ctx->aio:0, ctx->directio:1
2016/01/07 12:38:01[             ngx_output_chain_as_is,   309][yangya  [debug] 20090#20090: ngx_output_chain_as_is--- buf_special:0, in_file:1, directio:1, buf_in_mem:0,need_in_memory:0, need_in_temp:0, memory:0, mmap:0
2016/01/07 12:38:01[                    ngx_thread_read,   147]  [debug] 20090#20090: *14 thread read: fd:13, buf:080F3890, size:2340, offset:5111808
2016/01/07 12:38:01[           ngx_http_postpone_filter,   176]  [debug] 20090#20090: *14 http postpone filter "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931" 080F3440
2016/01/07 12:38:01[              ngx_http_write_filter,   208]  [debug] 20090#20090: *14 write new buf temporary:1 buf-in-file:0, buf->start:080F3890, buf->pos:080F3890, buf_size: 2340 file_pos: 0, in_file_size: 0
2016/01/07 12:38:01[              ngx_http_write_filter,   248]  [debug] 20090#20090: *14 http write filter: last:1 flush:0 size:2340
2016/01/07 12:38:01[              ngx_http_write_filter,   380]  [debug] 20090#20090: *14 http write filter limit 0
2016/01/07 12:38:01[           ngx_linux_sendfile_chain,   201][yangya  [debug] 20090#20090: *14 @@@@@@@@@@@@@@@@@@@@@@@begin ngx_linux_sendfile_chain @@@@@@@@@@@@@@@@@@@
2016/01/07 12:38:01[                         ngx_writev,   238]  [debug] 20090#20090: *14 writev: 2340 of 2340
2016/01/07 12:38:01[              ngx_http_write_filter,   386]  [debug] 20090#20090: *14 http write filter 00000000
2016/01/07 12:38:01[               ngx_http_copy_filter,   284]  [debug] 20090#20090: *14 http copy filter rc: 0, buffered:0 "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                    ngx_http_writer,  3118]  [debug] 20090#20090: *14 http writer output filter: 0, "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[                    ngx_http_writer,  3150]  [debug] 20090#20090: *14 http writer done: "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931"
2016/01/07 12:38:01[          ngx_http_finalize_request,  2592]  [debug] 20090#20090: *14 http finalize request rc: 0, "/download/nginx-1.9.2.rar?st=xhWL03HbtjrojpEAfiD6Mw&e=1452139931" a:1, c:1
2016/01/07 12:38:01[                ngx_event_del_timer,    39]  [debug] 20090#20090: *14 <ngx_http_finalize_request,  2831>  event timer del: 12: 442595277
2016/01/07 12:38:01[             ngx_http_set_keepalive,  3325]  [debug] 20090#20090: *14 set http keepalive handler
2016/01/07 12:38:01[              ngx_http_free_request,  3977]  [debug] 20090#20090: *14 http close request
2016/01/07 12:38:01[               ngx_http_log_handler,   267]  [debug] 20090#20090: *14 http log handler
2016/01/07 12:38:01[             ngx_http_set_keepalive,  3444]  [debug] 20090#20090: *14 hc free: 00000000 0
2016/01/07 12:38:01[             ngx_http_set_keepalive,  3456]  [debug] 20090#20090: *14 hc busy: 00000000 0
2016/01/07 12:38:01[            ngx_reusable_connection,  1177]  [debug] 20090#20090: *14 reusable connection: 1
2016/01/07 12:38:01[                ngx_event_add_timer,   100]  [debug] 20090#20090: *14 <   ngx_http_set_keepalive,  3532>  event timer add fd:12, expire-time:75 s, timer.key:442610452
2016/01/07 12:38:01[             ngx_http_set_keepalive,  3535]  [debug] 20090#20090: *14 post event B0793098
2016/01/07 12:38:01[           ngx_event_process_posted,    65]  [debug] 20090#20090: begin to run befor posted event B0793098
2016/01/07 12:38:01[           ngx_event_process_posted,    67]  [debug] 20090#20090: *14 delete posted event B0793098
2016/01/07 12:38:01[         ngx_http_keepalive_handler,  3555]  [debug] 20090#20090: *14 http keepalive handler
2016/01/07 12:38:01[                      ngx_unix_recv,   185]  [debug] 20090#20090: *14 recv() not ready (11: Resource temporarily unavailable)
2016/01/07 12:38:01[                      ngx_unix_recv,   204]  [debug] 20090#20090: *14 recv: fd:12 read-size:-2 of 1024, ready:0
2016/01/07 12:38:01[           ngx_trylock_accept_mutex,   405]  [debug] 20090#20090: accept mutex locked
2016/01/07 12:38:10[            ngx_event_expire_timers,   133]  [debug] 20092#20092: event timer del: -1: 442544857
2016/01/07 12:38:10[         ngx_http_file_cache_expire,  2018]  [debug] 20092#20092: http file cache expire
2016/01/07 12:38:10[                     ngx_shmtx_lock,   168]  [debug] 20092#20092: shmtx lock
2016/01/07 12:38:10[                   ngx_shmtx_unlock,   249]  [debug] 20092#20092: shmtx unlock
2016/01/07 12:38:10[                     ngx_shmtx_lock,   168]  [debug] 20092#20092: shmtx lock
2016/01/07 12:38:10[                   ngx_shmtx_unlock,   249]  [debug] 20092#20092: shmtx unlock
2016/01/07 12:38:10[        ngx_http_file_cache_manager,  2217]  [debug] 20092#20092: http file cache size: 2, max_size:262144
2016/01/07 12:38:10[                ngx_event_add_timer,   100]  [debug] 20092#20092: <ngx_cache_manager_process_handler,  1648>  event timer add fd:-1, expire-time:10 s, timer.key:442554857
^C
root@root:/usr/local/nginx/logs# 


//濡