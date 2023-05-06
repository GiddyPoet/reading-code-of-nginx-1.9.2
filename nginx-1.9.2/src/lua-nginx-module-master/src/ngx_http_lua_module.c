
/*
 * Copyright (C) Xiaozhe Wang (chaoslawful)
 * Copyright (C) Yichun Zhang (agentzh)
 */

/*
如何安装nginx_lua_module模块
2012-03-27 15:01 by 轩脉刃, 19397 阅读, 0 评论, 收藏, 编辑


摘要:

本文记录如何安装ngx_lua模块

nginx_lua_module是由淘宝的工程师清无（王晓哲）和春来（章亦春）所开发的nginx第三方模块,它能将lua语言嵌入到nginx配置中,从而使用lua就极大增强了nginx的能力
http://wiki.nginx.org/HttpLuaModule

正文:

1 下载luajit 2.0并安装

http://luajit.org/download.html

我是直接使用源码make && make install

所以lib和include是直接放在/usr/local/lib和usr/local/include

 

2 下载nginx源码，解压

注意版本号，如果机子上已经装了nginx，不想升级的话，请使用/to/nginx/sbin/nginx 