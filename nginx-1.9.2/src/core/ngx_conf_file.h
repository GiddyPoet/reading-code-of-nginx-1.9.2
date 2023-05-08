
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */
/*
#定义Nginx运行的用户和用户组
user www www;
 
#nginx进程数，建议设置为等于CPU总核心数。
worker_processes 8;
 
#全局错误日志定义类型，[ debug | info | notice | warn | error | crit ]
error_log ar/loginx/error.log info;
 
#进程文件
pid ar/runinx.pid;
 
#一个nginx进程打开的最多文件描述符数目，理论值应该是最多打开文件数（系统的值ulimit -n）与nginx进程数相除，但是nginx分配请求并不均匀，所以建议与ulimit -n的值保持一致。
worker_rlimit_nofile 65535;
 
#工作模式与连接数上限
events
{
#参考事件模型，use [ kqueue | rtsig | epoll | /dev/poll | select | poll ]; epoll模型是Linux 2.6以上版本内核中的高性能网络I/O模型，如果跑在FreeBSD上面，就用kqueue模型。
use epoll;
#单个进程最大连接数（最大连接数=连接数*进程数）
worker_connections 65535;
}
 
#设定http服务器
http
{
include mime.types; #文件扩展名与文件类型映射表
default_type application/octet-stream; #默认文件类型
#charset utf-8; #默认编码
server_names_hash_bucket_size 128; #服务器名字的hash表大小
client_header_buffer_size 32k; #上传文件大小限制
large_client_header_buffers 4 64k; #设定请求缓
client_max_body_size 8m; #设定请求缓
sendfile on; #开启高效文件传输模式，sendfile指令指定nginx是否调用sendfile函数来输出文件，对于普通应用设为 on，如果用来进行下载等应用磁盘IO重负载应用，可设置为off，以平衡磁盘与网络I/O处理速度，降低系统的负载。注意：如果图片显示不正常把这个改成off。
autoindex on; #开启目录列表访问，合适下载服务器，默认关闭。
tcp_nopush on; #防止网络阻塞
tcp_nodelay on; #防止网络阻塞
keepalive_timeout 120; #长连接超时时间，单位是秒
 
#FastCGI相关参数是为了改善网站的性能：减少资源占用，提高访问速度。下面参数看字面意思都能理解。
fastcgi_connect_timeout 300;
fastcgi_send_timeout 300;
fastcgi_read_timeout 300;
fastcgi_buffer_size 64k;
fastcgi_buffers 4 64k;
fastcgi_busy_buffers_size 128k;
fastcgi_temp_file_write_size 128k;
 
#gzip模块设置
gzip on; #开启gzip压缩输出
gzip_min_length 1k; #最小压缩文件大小
gzip_buffers 4 16k; #压缩缓冲区
gzip_http_version 1.0; #压缩版本（默认1.1，前端如果是squid2.5请使用1.0）
gzip_comp_level 2; #压缩等级
gzip_types text/plain application/x-javascript text/css application/xml;
#压缩类型，默认就已经包含textml，所以下面就不用再写了，写上去也不会有问题，但是会有一个warn。
gzip_vary on;
#limit_zone crawler $binary_remote_addr 10m; #开启限制IP连接数的时候需要使用
 
upstream blog.ha97.com {
#upstream的负载均衡，weight是权重，可以根据机器配置定义权重。weigth参数表示权值，权值越高被分配到的几率越大。
server 192.168.80.121:80 weight=3;
server 192.168.80.122:80 weight=2;
server 192.168.80.123:80 weight=3;
}
 
#虚拟主机的配置
server
{
#监听端口
listen 80;
#域名可以有多个，用空格隔开
server_name www.ha97.com ha97.com;
index index.html index.htm index.php;
root /data/www/ha97;
location ~ .*.(php|php5)?$
{
fastcgi_pass 127.0.0.1:9000;
fastcgi_index index.php;
include fastcgi.conf;
}
#图片缓存时间设置
location ~ .*.(gif|jpg|jpeg|png|bmp|swf)$
{
expires 10d;
}
#JS和CSS缓存时间设置
location ~ .*.(js|css)?$
{
expires 1h;
}
#日志格式设定
log_format access '$remote_addr - $remote_user [$time_local] "$request" '
'$status $body_bytes_sent "$http_referer" '
'"$http_user_agent" $http_x_forwarded_for';
#定义本虚拟主机的访问日志
access_log ar/loginx/ha97access.log access;
 
#对 "/" 启用反向代理
location / {
proxy_pass http://127.0.0.1:88;
proxy_redirect off;
proxy_set_header X-Real-IP $remote_addr;
#后端的Web服务器可以通过X-Forwarded-For获取用户真实IP
proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
#以下是一些反向代理的配置，可选。
proxy_set_header Host $host;
client_max_body_size 10m; #允许客户端请求的最大单文件字节数
client_body_buffer_size 128k; #缓冲区代理缓冲用户端请求的最大字节数，
proxy_connect_timeout 90; #nginx跟后端服务器连接超时时间(代理连接超时)
proxy_send_timeout 90; #后端服务器数据回传时间(代理发送超时)
proxy_read_timeout 90; #连接成功后，后端服务器响应时间(代理接收超时)
proxy_buffer_size 4k; #设置代理服务器（nginx）保存用户头信息的缓冲区大小
proxy_buffers 4 32k; #proxy_buffers缓冲区，网页平均在32k以下的设置
proxy_busy_buffers_size 64k; #高负荷下缓冲大小（proxy_buffers*2）
proxy_temp_file_write_size 64k;
#设定缓存文件夹大小，大于这个值，将从upstream服务器传
}
 
#设定查看Nginx状态的地址
location /NginxStatus {
stub_status on;
access_log on;
auth_basic "NginxStatus";
auth_basic_user_file confpasswd;
#htpasswd文件的内容可以用apache提供的htpasswd工具来产生。
}
 
#本地动静分离反向代理配置
#所有jsp的页面均交由tomcat或resin处理
location ~ .(jsp|jspx|do)?$ {
proxy_set_header Host $host;
proxy_set_header X-Real-IP $remote_addr;
proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
proxy_pass http://127.0.0.1:8080;
}
#所有静态文件由nginx直接读取不经过tomcat或resin
location ~ .*.(htm|html|gif|jpg|jpeg|png|bmp|swf|ioc|rar|zip|txt|flv|mid|doc|ppt|pdf|xls|mp3|wma)$
{ expires 15d; }
location ~ .*.(js|css)?$
{ expires 1h; }
}
}

*/

#ifndef _NGX_CONF_FILE_H_INCLUDED_
#define _NGX_CONF_FILE_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>


/*
 *        AAAA  number of arguments
 *      FF      command flags
 *    TT        command type, i.e. HTTP "location" or "server" command
 */

/*
表4-1  ngx_command_s结构体中type成员的取值及其意义
┏━━━━━━━━━┳━━━━━━━━━━┳━━━━━━━━━━━━━━━━━━━━━━━━━━┓
┃    type类型      ┃    type取值        ┃    意义                                            ┃
┣━━━━━━━━━╋━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━┫
┃                  ┃                    ┃  一般由NGX_CORE_MODULE类型的核心模块使用，         ┃
┃                  ┃                    ┃仅与下面的NGX_MAIN_CONF同时设置，表示模块需         ┃
┃  处理配置项时获  ┃NGX_DIRECT_CONF     ┃要解析不属于任何{}内的全局配置项。它实际上会指定   ┃
┃取当前配置块的方  ┃                    ┃set方法里的第3个参数conf的值，使之指向每个模块解    ┃
┃式                ┃                    ┃析全局配置项的配置结构体①                          ┃
┃                  ┣━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━┫
┃                  ┃NGX_ANY_CONF        ┃  目前未使用，设置与否均无意义                      ┃
┣━━━━━━━━━╋━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━┫
┃                  ┃                    ┃  配置项可以出现在全局配置中，即不属于任何{}配置   ┃
┃                  ┃NGX_MAIN_CONF       ┃                                                    ┃
┃                  ┃                    ┃块                                                  ┃
┃                  ┣━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━┫
┃                  ┃NGX_EVENT_CONF      ┃  配置项可以出现在events{}块内                      ┃
┃                  ┣━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━┫
┃                  ┃NGX_MAIL_MAIN_CONF  ┃  配置项可以出现在mail{}块或者imap{）块内           ┃
┃                  ┣━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━┫
┃                  ┃                    ┃  配置项可以出现在server{}块内，然而该server{}块    ┃
┃                  ┃NGX_MAIL_SRV_CONF   ┃                                                    ┃
┃                  ┃                    ┃必须属于mail{}块或者imap{}块                        ┃
┃                  ┣━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━┫
┃                  ┃NGX_HTTP_MAIN_CONF  ┃  配置项可以出现在http{}块内                        ┃
┃                  ┣━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━┫
┃                  ┃NGX_HTTP_SRV_CONF   ┃  配置项可以出现在server{）块肉，然而该server块必   ┃
┃                  ┃                    ┃须属于http{）块                                     ┃
┃  配置项可以在哪  ┃                    ┃                                                    ┃
┃                  ┣━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━┫
┃                  ┃                    ┃  配置项可以出现在location{}块内，然而该location块  ┃
┃些{）配置块中出现 ┃NGX_HTTP_LOC_CONF   ┃                                                    ┃
┃                  ┃                    ┃必须属于http{）块                                   ┃
┃                  ┣━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━┫
┃                  ┃                    ┃  配置项可以出现在upstream{}块内，然而该upstream    ┃
┃                  ┃NGX_HTTP_UPS_CONF   ┃                                                    ┃
┃                  ┃                    ┃块必须属于http{）块                                 ┃
┃                  ┣━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━┫
┃                  ┃                    ┃  配置项可以出现在server块内的if{}块中。目前仅有    ┃
┃                  ┃NGX_HTTP_SIF_CONF   ┃                                                    ┃
┃                  ┃                    ┃rewrite模块会使用，该if块必须属于http{）块          ┃
┃                  ┣━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━┫
┃                  ┃                    ┃  配置项可以出现在location块内的if{）块中。目前仅   ┃
┃                  ┃NGX_HTTP_LIF_CONF   ┃                                                    ┃
┃                  ┃                    ┃有rewrite模块会使用，该if块必须属于http{）块        ┃
┃                  ┣━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━┫
┃                  ┃                    ┃  配置项可以出现在limit_except{）块内，然而该limit- ┃
┃                  ┃NGX_HTTP_LMT_CONF   ┃                                                    ┃
┃                  ┃                    ┃except块必须属于http{）块                           ┃
┣━━━━━━━━━╋━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━┫
┃                  ┃NGX_CONF_NOARGS     ┃  配置项不携带任何参数                              ┃
┃                  ┣━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━┫
┃                  ┃NGX_CONF_TAKE1      ┃  配置项必须携带1个参数                             ┃
┃                  ┣━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━┫
┃                  ┃NGX_CONF_TAKE2      ┃  配置项必须携带2个参数                             ┃
┃                  ┣━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━┫
┃                  ┃NGX_CONF_TAKE3      ┃  配置项必须携带3个参数                             ┃
┃                  ┣━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━┫
┃                  ┃NGX_CONF_TAKE4      ┃  配置项必须携带4个参数                             ┃
┃                  ┣━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━┫
┃                  ┃NGX_CONF_TAKE5      ┃  配置项必须携带5个参数                             ┃
┃                  ┣━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━┫
┃  限制配置项的参  ┃NGX_CONF_TAKE6      ┃  酡置项必须携带6个参数                             ┃
┃数个数            ┃                    ┃                                                    ┃
┃                  ┣━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━┫
┃                  ┃NGX_CONF_TAKE7      ┃  配置项必须携带7个参数                             ┃
┃                  ┣━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━┫
┃                  ┃NGX_CONF_TAKE12     ┃  配置项可以携带1个参数或2个参数                    ┃
┃                  ┣━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━┫
┃                  ┃NGX_CONF_TAKE13     ┃  配置项可以携带1个参数或3个参数                    ┃
┃                  ┣━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━┫
┃                  ┃NGX_CONF_TAKE23     ┃  配置项可以携带2个参数或3个参数                    ┃
┃                  ┣━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━┫
┃                  ┃NGX_CONF_TAKE123    ┃  配置项可以携带1～3个参数                          ┃
┃                  ┣━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━┫
┃                  ┃NGX_CONF_TAKE1234   ┃  配置项可以携带1～4个参数                          ┃
┗━━━━━━━━━┻━━━━━━━━━━┻━━━━━━━━━━━━━━━━━━━━━━━━━━┛
┏━━━━━━━━━┳━━━━━━━━━━━┳━━━━━━━━━━━━━━━━━━━━━━━━━━┓
┃    type类型      ┃    type取值          ┃    意义                                            ┃
┣━━━━━━━━━╋━━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━┫
┃                  ┃NGX CONF ARGS NUMBER  ┃  目前未使用，无意义                                ┃
┃                  ┣━━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━┫
┃                  ┃                      ┃  配置项定义了一种新的{）块。例如，http、server、   ┃
┃                  ┃NGX CONF BLOCK        ┃location等配置，它们的type都必须定义为NGX二CONF     ┃
┃                  ┃                      ┃ BLOCK                                              ┃
┃                  ┣━━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━┫
┃                  ┃ NGX CONF ANY         ┃  不验证配置项携带的参数个数                        ┃
┃                  ┣━━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━┫
┃                  ┃                      ┃  配置项携带的参数只能是1个，并且参数的值只能是     ┃
┃                  ┃NGX CONF FLAG         ┃                                                    ┃
┃                  ┃                      ┃on或者off                                           ┃
┃                  ┣━━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━┫
┃                  ┃NGX CONF IMORE        ┃  配置项携带的参数个数必须超过1个                   ┃
┃                  ┣━━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━┫
┃                  ┃NGX CONF 2MORE        ┃  配置项携带的参数个数必须超过2个                   ┃
┃  限制配置项后的  ┃                      ┃                                                    ┃
┃                  ┣━━━━━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━┫
┃参数出现的形式    ┃                      ┃  表示当前配置项可以出现在任意块中（包括不属于任    ┃
┃                  ┃                      ┃何块的全局配置），它仅用于配合其他配置项使用。type  ┃
┃                  ┃                      ┃中未加NGX―CONF_ MULTI时，如果一个配置项出现在      ┃
┃                  ┃                      ┃type成员未标明的配置块中，那么Nginx会认为该配置     ┃
┃                  ┃                      ┃项非法，最后将导致Nginx启动失败。但如果type中加     ┃
┃                  ┃                      ┃入了NGX CONF- MULTI，则认为该配置项一定是合法       ┃
┃                  ┃NGX CONF MULTI        ┃                                                    ┃
┃                  ┃                      ┃的，然而又会有两种不同的结果：①如果配置项出现在    ┃
┃                  ┃                      ┃type指示的块中，则会调用set方法解析醌置项；②如果   ┃
┃                  ┃                      ┃配置项没有出现在type指示的块中，则不对该配置项做    ┃
┃                  ┃                      ┃任何处理。因此，NGX―CONF―MULTI会使得配置项出      ┃
┃                  ┃                      ┃现在未知块中时不会出错。目前，还没有官方模块使用过  ┃
┃                  ┃                      ┃NGX―CONF―MULTI                                    ┃
┗━━━━━━━━━┻━━━━━━━━━━━┻━━━━━━━━━━━━━━━━━━━━━━━━━━┛
①每个进程中都有一个唯一的ngx_cycle_t核心结构体，它有一个成员conf_ctx维护着所有模块的配置结构体，
  其类型是voi扩”4。conf ctx意义为首先指向一个成员皆为指针的数组，其中每个成员指针又指向另外一个
  成员皆为指针的数组，第2个子数组中的成员指针才会指向各模块生成的配置结构体。这正是为了事件模
  块、http模块、mail模块而设计的，这有利于不同于NGX CORE MODULE类型的
  特定模块解析配置项。然而，NGX CORE―MODULE类型的核心模块解析配置项时，配置项一定是全局的，
  不会从属于任何{）配置块的，它不需要上述这种双数组设计。解析标识为NGX DIRECT CONF类型的配
  置项时，会把void+十++类型的conf ctx强制转换为void~+，也就是说，此时，在conf ctx指向的指针数组
  中，每个成员指针不再指向其他数组，直接指向核心模块生成的配置鲒构体。因此，NGX_ DIRECT__ CONF
  仅由NGX CORE MODULE类型的核心模块使用，而且配置项只应该出现在全局配置中。
    注意  如果HTTP模块中定义的配置项在nginx.conf配置文件中实际出现的位置和参数
格式与type的意义不符，那么Nginx在启动时会报错。
*/

/*
以下这些宏用于限制配置项的参数个数

NGX_CONF_NOARGS：配置项不允许带参数
NGX_CONF_TAKE1：配置项可以带1个参数
NGX_CONF_TAKE2：配置项可以带2个参数
NGX_CONF_TAKE3：配置项可以带3个参数
NGX_CONF_TAKE4：配置项可以带4个参数
NGX_CONF_TAKE5：配置项可以带5个参数
NGX_CONF_TAKE6：配置项可以带6个参数
NGX_CONF_TAKE7：配置项可以带7个参数
NGX_CONF_TAKE12：配置项可以带1或2个参数
NGX_CONF_TAKE13：配置项可以带1或3个参数
NGX_CONF_TAKE23：配置项可以带2或3个参数
NGX_CONF_TAKE123：配置项可以带1-3个参数
NGX_CONF_TAKE1234：配置项可以带1-4个参数
*/
#define NGX_CONF_NOARGS      0x00000001
#define NGX_CONF_TAKE1       0x00000002
#define NGX_CONF_TAKE2       0x00000004
#define NGX_CONF_TAKE3       0x00000008
#define NGX_CONF_TAKE4       0x00000010
#define NGX_CONF_TAKE5       0x00000020
#define NGX_CONF_TAKE6       0x00000040
#define NGX_CONF_TAKE7       0x00000080
#define NGX_CONF_MAX_ARGS    8
#define NGX_CONF_TAKE12      (NGX_CONF_TAKE1|NGX_CONF_TAKE2)
#define NGX_CONF_TAKE13      (NGX_CONF_TAKE1|NGX_CONF_TAKE3)

#define NGX_CONF_TAKE23      (NGX_CONF_TAKE2|NGX_CONF_TAKE3)

#define NGX_CONF_TAKE123     (NGX_CONF_TAKE1|NGX_CONF_TAKE2|NGX_CONF_TAKE3)
#define NGX_CONF_TAKE1234    (NGX_CONF_TAKE1|NGX_CONF_TAKE2|NGX_CONF_TAKE3   \
                              |NGX_CONF_TAKE4)

#define NGX_CONF_ARGS_NUMBER 0x000000ff
/*
以下这些宏用于限制配置项参数形式

NGX_CONF_BLOCK：配置项定义了一种新的{}块，如：http、server等配置项。
NGX_CONF_ANY：不验证配置项携带的参数个数。
NGX_CONF_FLAG：配置项只能带一个参数，并且参数必需是on或者off。
NGX_CONF_1MORE：配置项携带的参数必需超过一个。
NGX_CONF_2MORE：配置项携带的参数必需超过二个。
*/
#define NGX_CONF_BLOCK       0x00000100
#define NGX_CONF_FLAG        0x00000200
#define NGX_CONF_ANY         0x00000400
#define NGX_CONF_1MORE       0x00000800
#define NGX_CONF_2MORE       0x00001000
#define NGX_CONF_MULTI       0x00000000  /* compatibility */

//使用全局配置，主要包括以下命令//ngx_core_commands  ngx_openssl_commands  ngx_google_perftools_commands   ngx_regex_commands  ngx_thread_pool_commands
#define NGX_DIRECT_CONF      0x00010000 //都是和NGX_MAIN_CONF一起使用

/*
NGX_MAIN_CONF：配置项可以出现在全局配置中，即不属于任何{}配置块。
NGX_EVENT_CONF：配置项可以出现在events{}块内。
NGX_HTTP_MAIN_CONF： 配置项可以出现在http{}块内。
NGX_HTTP_SRV_CONF:：配置项可以出现在server{}块内，该server块必需属于http{}块。
NGX_HTTP_LOC_CONF：配置项可以出现在location{}块内，该location块必需属于server{}块。
NGX_HTTP_UPS_CONF： 配置项可以出现在upstream{}块内，该location块必需属于http{}块。
NGX_HTTP_SIF_CONF：配置项可以出现在server{}块内的if{}块中。该if块必须属于http{}块。
NGX_HTTP_LIF_CONF: 配置项可以出现在location{}块内的if{}块中。该if块必须属于http{}块。
NGX_HTTP_LMT_CONF: 配置项可以出现在limit_except{}块内,该limit_except块必须属于http{}块。
*/ 

//支持NGX_MAIN_CONF | NGX_DIRECT_CONF的包括:
//ngx_core_commands  ngx_openssl_commands  ngx_google_perftools_commands   ngx_regex_commands  ngx_thread_pool_commands
//这些一般是一级配置里面的配置项，http{}外的

/*
总结，一般一级配置(http{}外的配置项)一般属性包括NGX_MAIN_CONF|NGX_DIRECT_CONF。http events等这一行的配置属性,全局配置项worker_priority等也属于这个行列
包括NGX_MAIN_CONF不包括NGX_DIRECT_CONF
*/
#define NGX_MAIN_CONF        0x01000000  


/*
配置类型模块是唯一一种只有1个模块的模块类型。配置模块的类型叫做NGX_CONF_MODULE，它仅有的模块叫做ngx_conf_module，这是Nginx最
底层的模块，它指导着所有模块以配置项为核心来提供功能。因此，它是其他所有模块的基础。
*/
#define NGX_ANY_CONF         0x0F000000


#define NGX_CONF_UNSET       -1
#define NGX_CONF_UNSET_UINT  (ngx_uint_t) -1
#define NGX_CONF_UNSET_PTR   (void *) -1
#define NGX_CONF_UNSET_SIZE  (size_t) -1
#define NGX_CONF_UNSET_MSEC  (ngx_msec_t) -1


#define NGX_CONF_OK          NULL
#define NGX_CONF_ERROR       (void *) -1

#define NGX_CONF_BLOCK_START 1
#define NGX_CONF_BLOCK_DONE  2
#define NGX_CONF_FILE_DONE   3

//GX_CORE_MODULE类型的核心模块解析配置项时，配置项一定是全局的，
/*
NGX_CORE_MODULE主要包括以下模块:
ngx_core_module  ngx_events_module  ngx_http_module  ngx_errlog_module  ngx_mail_module  
ngx_regex_module  ngx_stream_module  ngx_thread_pool_module
*/

//所有的核心模块NGX_CORE_MODULE对应的上下文ctx为ngx_core_module_t，子模块，例如http{} NGX_HTTP_MODULE模块对应的为上下文为ngx_http_module_t
//events{} NGX_EVENT_MODULE模块对应的为上下文为ngx_event_module_t
/*
Nginx还定义了一种基础类型的模块：核心模块，它的模块类型叫做NGX_CORE_MODULE。目前官方的核心类型模块中共有6个具体模块，分别
是ngx_core_module、ngx_errlog_module、ngx_events_module、ngx_openssl_module、ngx_http_module、ngx_mail_module模块
*/
#define NGX_CORE_MODULE      0x45524F43  /* "CORE" */ //二级模块类型http模块个数，见ngx_http_block  ngx_max_module为NGX_CORE_MODULE(一级模块类型)类型的模块数

//NGX_CONF_MODULE只包括:ngx_conf_module
#define NGX_CONF_MODULE      0x464E4F43  /* "CONF" */


#define NGX_MAX_CONF_ERRSTR  1024
/*
Nginx安装完毕后，会有响应的安装目录，安装目录里nginx.conf为nginx的主配置文件，ginx主配置文件分为4部分，main（全局配置）、server（主机设置）、upstream（负载均衡服务器设）和location（URL匹配特定位置的设置），这四者关系为：server继承main，location继承server，upstream既不会继承其他设置也不会被继承。

一、Nginx的main（全局配置）文件

[root@rhel6u3-7 server]# vim /usr/local/nginx/conf/nginx.conf 
user nginx nginx; //指定nginx运行的用户及用户组为nginx，默认为nobody 
worker_processes 2； //开启的进程数，一般跟逻辑cpu核数一致 
error_log logs/error.log notice; //定于全局错误日志文件，级别以notice显示。还有debug、info、warn、error、crit模式，debug输出最多，crit输出最少，更加实际环境而定。 
pid logs/nginx.pid; //指定进程id的存储文件位置 
worker_rlimit_nofile 65535; //指定一个nginx进程打开的最多文件描述符数目，受系统进程的最大打开文件数量限制 
events { 
use epoll; 设置工作模式为epoll，除此之外还有select、poll、kqueue、rtsig和/dev/poll模式 
worker_connections 65535; //定义每个进程的最大连接数 受系统进程的最大打开文件数量限制 
} 
…….

[root@rhel6u3-7 server]# cat /proc/cpuinfo | grep "processor" | wc 