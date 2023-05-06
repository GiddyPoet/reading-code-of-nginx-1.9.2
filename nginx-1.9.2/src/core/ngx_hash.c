
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>

/*
To quickly process static sets of data such as server names, map directive’s values, MIME types, names of request header strings, nginx 
uses hash tables. During the start and each re-configuration nginx selects the minimum possible sizes of hash tables such that the bucket 
size that stores keys with identical hash values does not exceed the configured parameter (hash bucket size). The size of a table is 
expressed in buckets. The adjustment is continued until the table size exceeds the hash max size parameter. Most hashes have the 
corresponding directives that allow changing these parameters, for example, for the server names hash they are server_names_hash_max_size 
and server_names_hash_bucket_size. 

The hash bucket size parameter is aligned to the size that is a multiple of the processor’s cache line size. This speeds up key search in 
a hash on modern processors by reducing the number of memory accesses. If hash bucket size is equal to one processor’s cache line size 
then the number of memory accesses during the key search will be two in the worst case ― first to compute the bucket address, and second 
during the key search inside the bucket. Therefore, if nginx emits the message requesting to increase either hash max size or hash bucket 
size then the first parameter should first be increased. 
*/
void *
ngx_hash_find(ngx_hash_t *hash, ngx_uint_t key, u_char *name, size_t len)
{
    ngx_uint_t       i;
    ngx_hash_elt_t  *elt;

#if 0
    ngx_log_error(NGX_LOG_ALERT, ngx_cycle->log, 0, "hf:\"%*s\"", len, name);
#endif

    elt = hash->buckets[key % hash->size];

    if (elt == NULL) {
        return NULL;
    }

    while (elt->value) {
        if (len != (size_t) elt->len) {
            goto next;
        }

        for (i = 0; i < len; i++) {
            if (name[i] != elt->name[i]) {
                goto next;
            }
        }

        return elt->value;

    next:

        elt = (ngx_hash_elt_t *) ngx_align_ptr(&elt->name[0] + elt->len,
                                               sizeof(void *));
        continue;
    }

    return NULL;
}

/*
nginx为了处理带有通配符的域名的匹配问题，实现了ngx_hash_wildcard_t这样的hash表。他可以支持两种类型的带有通配符的域名。一种是通配符在前的，
例如：“*.abc.com”，也可以省略掉星号，直接写成”.abc.com”。这样的key，可以匹配www.abc.com，qqq.www.abc.com之类的。另外一种是通配符在末
尾的，例如：“mail.xxx.*”，请特别注意通配符在末尾的不像位于开始的通配符可以被省略掉。这样的通配符，可以匹配mail.xxx.com、mail.xxx.com.cn、
mail.xxx.net之类的域名。

有一点必须说明，就是一个ngx_hash_wildcard_t类型的hash表只能包含通配符在前的key或者是通配符在后的key。不能同时包含两种类型的通配符
的key。ngx_hash_wildcard_t类型变量的构建是通过函数ngx_hash_wildcard_init完成的，而查询是通过函数ngx_hash_find_wc_head或者
ngx_hash_find_wc_tail来做的。ngx_hash_find_wc_head是查询包含通配符在前的key的hash表的，而ngx_hash_find_wc_tail是查询包含通配符在后的key的hash表的。
*/
void *
ngx_hash_find_wc_head(ngx_hash_wildcard_t *hwc, u_char *name, size_t len)
{
    void        *value;
    ngx_uint_t   i, n, key;

#if 0
    ngx_log_error(NGX_LOG_ALERT, ngx_cycle->log, 0, "wch:\"%*s\"", len, name);
#endif

    n = len;
    
    //从后往前搜索第一个dot，则n 到 len-1 即为关键字中最后一个 子关键字
    while (n) { //name中最后面的字符串，如 AA.BB.CC.DD，则这里获取到的就是DD
        if (name[n - 1] == '.') {
            break;
        }

        n--;
    }

    key = 0;
    
    //n 到 len-1 即为关键字中最后一个 子关键字，计算其hash值
    for (i = n; i < len; i++) {
        key = ngx_hash(key, name[i]);
    }

#if 0
    ngx_log_error(NGX_LOG_ALERT, ngx_cycle->log, 0, "key:\"%ui\"", key);
#endif

    //调用普通查找找到关键字的value（用户自定义数据指针）
    value = ngx_hash_find(&hwc->hash, key, &name[n], len - n);

#if 0
    ngx_log_error(NGX_LOG_ALERT, ngx_cycle->log, 0, "value:\"%p\"", value);
#endif

    if (value) {

        /*
         * the 2 low bits of value have the special meaning:
         *     00 - value is data pointer for both "example.com"
         *          and "*.example.com";
         *     01 - value is data pointer for "*.example.com" only;
         *     10 - value is pointer to wildcard hash allowing
         *          both "example.com" and "*.example.com";
         *     11 - value is pointer to wildcard hash allowing
         *          "*.example.com" only.
         */

        if ((uintptr_t) value & 2) {

            if (n == 0) { //搜索到了最后一个子关键字且没有通配符，如"example.com"的example

                /* "example.com" */

                if ((uintptr_t) value & 1) {//value低两位为11，仅为"*.example.com"的指针，这里没有通配符，没招到，返回NULL
                    return NULL;
                }
             //value低两位为10，为"example.com"的指针，value就在下一级的ngx_hash_wildcard_t 的value中，去掉携带的低2位11    参考ngx_hash_wildcard_init
                hwc = (ngx_hash_wildcard_t *)
                                          ((uintptr_t) value & (uintptr_t) ~3);
                return hwc->value;
            }

            //还未搜索完，低两位为11或10，继续去下级ngx_hash_wildcard_t中搜索
            hwc = (ngx_hash_wildcard_t *) ((uintptr_t) value & (uintptr_t) ~3); //把最低的两个地址为清0还回去，参考ngx_hash_wildcard_init

            //继续搜索 关键字中剩余部分，如"example.com"，搜索 0 到 n -1 即为 example
            value = ngx_hash_find_wc_head(hwc, name, n - 1);

            if (value) {//若找到，则返回
                return value;
            }

            return hwc->value; //低两位为00 找到，即为wc->value
        }

        if ((uintptr_t) value & 1) { //低两位为01

            if (n == 0) { //关键字没有通配符，错误返回空

                /* "example.com" */

                return NULL;
            }

            return (void *) ((uintptr_t) value & (uintptr_t) ~3);//有通配符，直接返回
        }

        return value; //低两位为00，直接返回
    }

    return hwc->value;
}

/*
nginx为了处理带有通配符的域名的匹配问题，实现了ngx_hash_wildcard_t这样的hash表。他可以支持两种类型的带有通配符的域名。一种是通配符在前的，
例如：“*.abc.com”，也可以省略掉星号，直接写成”.abc.com”。这样的key，可以匹配www.abc.com，qqq.www.abc.com之类的。另外一种是通配符在末
尾的，例如：“mail.xxx.*”，请特别注意通配符在末尾的不像位于开始的通配符可以被省略掉。这样的通配符，可以匹配mail.xxx.com、mail.xxx.com.cn、
mail.xxx.net之类的域名。

有一点必须说明，就是一个ngx_hash_wildcard_t类型的hash表只能包含通配符在前的key或者是通配符在后的key。不能同时包含两种类型的通配符
的key。ngx_hash_wildcard_t类型变量的构建是通过函数ngx_hash_wildcard_init完成的，而查询是通过函数ngx_hash_find_wc_head或者
ngx_hash_find_wc_tail来做的。ngx_hash_find_wc_head是查询包含通配符在前的key的hash表的，而ngx_hash_find_wc_tail是查询包含通配符在后的key的hash表的。

hinit: 构造一个通配符hash表的一些参数的一个集合。关于该参数对应的类型的说明，请参见ngx_hash_t类型中ngx_hash_init函数的说明。 

names: 构造此hash表的所有的通配符key的数组。特别要注意的是这里的key已经都是被预处理过的。例如：“*.abc.com”或者“.abc.com”
被预处理完成以后，变成了“com.abc.”。而“mail.xxx.*”则被预处理为“mail.xxx.”。为什么会被处理这样？这里不得不简单地描述一下
通配符hash表的实现原理。当构造此类型的hash表的时候，实际上是构造了一个hash表的一个“链表”，是通过hash表中的key“链接”起来的。
比如：对于“*.abc.com”将会构造出2个hash表，第一个hash表中有一个key为com的表项，该表项的value包含有指向第二个hash表的指针，
而第二个hash表中有一个表项abc，该表项的value包含有指向*.abc.com对应的value的指针。那么查询的时候，比如查询www.abc.com的时候，
先查com，通过查com可以找到第二级的hash表，在第二级hash表中，再查找abc，依次类推，直到在某一级的hash表中查到的表项对应的value对
应一个真正的值而非一个指向下一级hash表的指针的时候，查询过程结束。这里有一点需要特别注意的，就是names数组中元素的value所对应的
值（也就是真正的value所在的地址）必须是能被4整除的，或者说是在4的倍数的地址上是对齐的。因为这个value的值的低两位bit是有用的，
所以必须为0。如果不满足这个条件，这个hash表查询不出正确结果。 

nelts: names数组元素的个数。 

*/
/* 
@hwc  表示支持通配符的哈希表的结构体   
@name 表示实际关键字地址  
@len  表示实际关键字长度   

ngx_hash_find_wc_tail与前置通配符查找差不多，这里value低两位仅有两种标志，更加简单：

00 - value 是指向 用户自定义数据
11 - value的指向下一个哈希表 
*/
void *
ngx_hash_find_wc_tail(ngx_hash_wildcard_t *hwc, u_char *name, size_t len)
{
    void        *value;
    ngx_uint_t   i, key;

#if 0
    ngx_log_error(NGX_LOG_ALERT, ngx_cycle->log, 0, "wct:\"%*s\"", len, name);
#endif

    key = 0;

    //从前往后搜索第一个dot，则0 到 i 即为关键字中第一个 子关键字
    for (i = 0; i < len; i++) {
        if (name[i] == '.') {
            break;
        }

        key = ngx_hash(key, name[i]); //计算哈希值
    }

    if (i == len) {  //没有通配符，返回NULL
        return NULL;
    }

#if 0
    ngx_log_error(NGX_LOG_ALERT, ngx_cycle->log, 0, "key:\"%ui\"", key);
#endif

    value = ngx_hash_find(&hwc->hash, key, name, i); //调用普通查找找到关键字的value（用户自定义数据指针）

#if 0
    ngx_log_error(NGX_LOG_ALERT, ngx_cycle->log, 0, "value:\"%p\"", value);
#endif

    /*
    还记得上节在ngx_hash_wildcard_init中，用value指针低2位来携带信息吗？其是有特殊意义的，如下：  
    * 00 - value 是数据指针   
    * 11 - value的指向下一个哈希表   
    */
    if (value) {

        /*
         * the 2 low bits of value have the special meaning:
         *     00 - value is data pointer;
         *     11 - value is pointer to wildcard hash allowing "example.*".
         */

        if ((uintptr_t) value & 2) {//低2位为11，value的指向下一个哈希表，递归搜索

            i++;

            hwc = (ngx_hash_wildcard_t *) ((uintptr_t) value & (uintptr_t) ~3);

            value = ngx_hash_find_wc_tail(hwc, &name[i], len - i);

            if (value) { //找到低两位00，返回
                return value;
            }

            return hwc->value; //找打低两位11，返回hwc->value
        }

        return value;
    }

    return hwc->value; //低2位为00，直接返回数据
}

//从hash表中查找对应的key - name
void *
ngx_hash_find_combined(ngx_hash_combined_t *hash, ngx_uint_t key, u_char *name,
    size_t len)
{
    void  *value;

    if (hash->hash.buckets) {  //在普通hash表中查找
        value = ngx_hash_find(&hash->hash, key, name, len);

        if (value) {
            return value;
        }
    }

    if (len == 0) {
        return NULL;
    }

    if (hash->wc_head && hash->wc_head->hash.buckets) { //在前置通配符哈希表中查找
        value = ngx_hash_find_wc_head(hash->wc_head, name, len);

        if (value) {
            return value;
        }
    }

    if (hash->wc_tail && hash->wc_tail->hash.buckets) { //在后置通配符哈希表中查找
        value = ngx_hash_find_wc_tail(hash->wc_tail, name, len);

        if (value) {
            return value;
        }
    }

    return NULL;
}

/*
NGX_HASH_ELT_SIZE宏用来计算ngx_hash_elt_t结构大小，定义如下。
在32位平台上，sizeof(void*)=4，(name)->key.len即是ngx_hash_elt_t结构中name数组保存的内容的长度，其中的"+2"是要加上该结构中len字段(u_short类型)的大小。
*/
#define NGX_HASH_ELT_SIZE(name)                                               \
    (sizeof(void *) + ngx_align((name)->key.len + 2, sizeof(void *)))

/*
其names参数是ngx_hash_key_t结构的数组，即键-值对<key,value>数组，nelts表示该数组元素的个数

该函数初始化的结果就是将names数组保存的键-值对<key,value>，通过hash的方式将其存入相应的一个或多个hash桶(即代码中的buckets)中，
该hash过程用到的hash函数一般为ngx_hash_key_lc等。hash桶里面存放的是ngx_hash_elt_t结构的指针(hash元素指针)，该指针指向一个基本
连续的数据区。该数据区中存放的是经hash之后的键-值对<key',value'>，即ngx_hash_elt_t结构中的字段<name,value>。每一个这样的数据
区存放的键-值对<key',value'>可以是一个或多个。
*/ //ngx_hash_init中names数组存入hash桶前，其结构是ngx_hash_key_t形式，在往hash桶里面存数据的时候，会把ngx_hash_key_t里面的成员拷贝到ngx_hash_elt_t中相应成员
//源代码，比较长，总的流程即为：预估需要的桶数量 