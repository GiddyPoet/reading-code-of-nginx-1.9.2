
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_SLAB_H_INCLUDED_
#define _NGX_SLAB_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>


typedef struct ngx_slab_page_s  ngx_slab_page_t;
//图形化理解参考:http://blog.csdn.net/u013009575/article/details/17743261
struct ngx_slab_page_s { //初始化赋值在ngx_slab_init
    //多种情况，多个用途  
    //当需要分配新的页的时候，分配N个页ngx_slab_page_s结构中第一个页的slab表示这次一共分配了多少个页 //标记这是连续分配多个page，并且我不是首page，例如一次分配3个page,分配的page为[1-3]，则page[1].slab=3  page[2].slab=page[3].slab=NGX_SLAB_PAGE_BUSY记录
    //如果OBJ<128一个页中存放的是多个obj(例如128个32字节obj),则slab记录里面的obj的大小，见ngx_slab_alloc_locked
    //如果obj移位大小为ngx_slab_exact_shift，也就是obj128字节，page->slab = 1;page->slab存储obj的bitmap,例如这里为1，表示说第一个obj分配出去了   见ngx_slab_alloc_locked
    //如果obj移位大小为ngx_slab_exact_shift，也就是obj>128字节，page->slab = ((uintptr_t) 1 << NGX_SLAB_MAP_SHIFT) | shift;//大于128，也就是至少256