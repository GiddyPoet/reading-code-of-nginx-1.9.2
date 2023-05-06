/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>


#if (NGX_THREADS)
#include <ngx_thread_pool.h>
static void ngx_thread_read_handler(void *data, ngx_log_t *log);
#endif


#if (NGX_HAVE_FILE_AIO)

/*
文件异步IO
    事件驱动模块都是在处理网络事件，而没有涉及磁盘上文件的操作。本
节将讨论Linux内核2.6.2x之后版本中支持的文件异步I/O，以及ngx_epoll_module模块是
如何与文件异步I/O配合提供服务的。这里提到的文件异步I/O并不是glibc库提供的文件异
步I/O。glibc库提供的异步I/O是基于多线程实现的，它不是真正意义上的异步I/O。而本节
说明的异步I/O是由Linux内核实现，只有在内核中成功地完成了磁盘操作，内核才会通知
进程，进而使得磁盘文件的处理与网络事件的处理同样高效。
    使用这种方式的前提是Linux内核版本中必须支持文件异步I/O。当然，它带来的好处
也非常明显，Nginx把读取文件的操作异步地提交给内核后，内核会通知I/O设备独立地执
行操作，这样，Nginx进程可以继续充分地占用CPU。而且，当大量读事件堆积到I/O设备
的队列中时，将会发挥出内核中“电梯算法”的优势，从而降低随机读取磁盘扇区的成本。
    注意Linux内核级别的文件异步I/O是不支持缓存操作的，也就是说，即使需要操作
的文件块在Linux文件缓存中存在，也不会通过读取、更改缓存中的文件块来代替实际对磁
盘的操作，虽然从阻塞worker进程的角度上来说有了很大好转，但是对单个请求来说，还是
有可能降低实际处理的速度，因为原先可以从内存中快速获取的文件块在使用了异步I/O后
则一定会从磁盘上读取。异步文件I/O是把“双刃剑”，关键要看使用场景，如果大部分用户
请求对文件的操作都会落到文件缓存中，那么不要使用异步I/O，反之则可以试着使用文件
异步I/O，看一下是否会为服务带来并发能力上的提升。
    目前，Nginx仅支持在读取文件时使用异步I/O，因为正常写入文件时往往是写入内存
中就立刻返回，效率很高，而使用异步I/O写入时速度会明显下降。
文件异步AIO优点:
        异步I/O是由Linux内核实现，只有在内核中成功地完成了磁盘操作，内核才会通知
    进程，进而使得磁盘文件的处理与网络事件的处理同样高效。这样就不会阻塞worker进程。
缺点:
        不支持缓存操作的，也就是说，即使需要操作的文件块在Linux文件缓存中存在，也不会通过读取、
    更改缓存中的文件块来代替实际对磁盘的操作。有可能降低实际处理的速度，因为原先可以从内存中快速
    获取的文件块在使用了异步I/O后则一定会从磁盘上读取
究竟是选择异步I/O还是普通I/O操作呢?
        异步文件I/O是把“双刃剑”，关键要看使用场景，如果大部分用户
    请求对文件的操作都会落到文件缓存中，那么不要使用异步I/O，反之则可以试着使用文件
    异步I/O，看一下是否会为服务带来并发能力上的提升。
        目前，Nginx仅支持在读取文件时使用异步I/O，因为正常写入文件时往往是写入内存
    中就立刻返回，效率很高，而使用异步I/O写入时速度会明显下降。异步I/O不支持写操作，因为
    异步I/O无法利用缓存，而写操作通常是落到缓存上，linux会自动将文件中缓存中的数据写到磁盘
    
    普通文件读写过程:
    正常的系统调用read/write的流程是怎样的呢？
    - 读取：内核缓存有需要的文件数据:内核缓冲区->用户缓冲区;没有:硬盘->内核缓冲区->用户缓冲区;
    - 写回：数据会从用户地址空间拷贝到操作系统内核地址空间的页缓存中去，这是write就会直接返回，操作系统会在恰当的时机写入磁盘，这就是传说中的
*/
//direct AIO可以参考http://blog.csdn.net/bengda/article/details/21871413

ngx_uint_t  ngx_file_aio = 1; //如果创建ngx_eventfd失败，置0，表示不支持AIO

#endif


ssize_t
ngx_read_file(ngx_file_t *file, u_char *buf, size_t size, off_t offset)
{
    ssize_t  n;

    ngx_log_debug5(NGX_LOG_DEBUG_CORE, file->log, 0,
                   "read file %V: %d, %p, %uz, %O", &file->name, file->fd, buf, size, offset);

#if (NGX_HAVE_PREAD)  //在配置脚本中赋值auto/unix:ngx_feature_name="NGX_HAVE_PREAD"

    n = pread(file->fd, buf, size, offset);//pread() 从文件 fd 指定的偏移 offset (相对文件开头) 上读取 count 个字节到 buf 开始位置。文件当前位置偏移保持不变。 

    if (n == -1) {
        ngx_log_error(NGX_LOG_CRIT, file->log, ngx_errno,
                      "pread() \"%s\" failed", file->name.data);
        return NGX_ERROR;
    }

#else

    if (file->sys_offset != offset) {
        if (lseek(file->fd, offset, SEEK_SET) == -1) {
            ngx_log_error(NGX_LOG_CRIT, file->log, ngx_errno,
                          "lseek() \"%s\" failed", file->name.data);
            return NGX_ERROR;
        }

        file->sys_offset = offset;
    }

    n = read(file->fd, buf, size);

    if (n == -1) {
        ngx_log_error(NGX_LOG_CRIT, file->log, ngx_errno,
                      "read() \"%s\" failed", file->name.data);
        return NGX_ERROR;
    }

    file->sys_offset += n;

#endif

    file->offset += n;//每读n字节，文件读取偏移量就加n

    return n;
}


#if (NGX_THREADS)

//ngx_thread_read中创建空间和赋值
typedef struct {
    ngx_fd_t     fd; //文件fd
    u_char      *buf; //读取文件内容到该buf中
    size_t       size; //读取文件内容大小
    off_t        offset; //从文件offset开始处读取size字节到buf中

    size_t       read; //通过ngx_thread_read_handler读取到的字节数
    ngx_err_t    err; //ngx_thread_read_handler读取返回后的错误信息
} ngx_thread_read_ctx_t; //见ngx_thread_read，该结构由ngx_thread_task_t->ctx指向

//第一次进来的时候表示开始把读任务加入线程池中处理，表示正在开始读，第二次进来的时候表示数据已经通过notify_epoll通知读取完毕，可以处理了，第一次返回NAX_AGAIN
//第二次放回线程池中的线程处理读任务读取到的字节数
ssize_t
ngx_thread_read(ngx_thread_task_t **taskp, ngx_file_t *file, u_char *buf,
    size_t size, off_t offset, ngx_pool_t *pool)
{
    /*
        该函数一般会进来两次，第一次是通过原始数据发送触发走到这里，这时候complete = 0，第二次是当线程池读取数据完成，则会通过
        ngx_thread_pool_handler->ngx_http_copy_thread_event_handler->ngx_http_request_handler->ngx_http_writer在次走到这里
     */
    ngx_thread_task_t      *task;
    ngx_thread_read_ctx_t  *ctx;

    ngx_log_debug4(NGX_LOG_DEBUG_CORE, file->log, 0,
                   "thread read: fd:%d, buf:%p, size:%uz, offset:%O",
                   file->fd, buf, size, offset);

    task = *taskp;

    if (task == NULL) {
        task = ngx_thread_task_alloc(pool, sizeof(ngx_thread_read_ctx_t));
        if (task == NULL) {
            return NGX_ERROR;
        }

        task->handler = ngx_thread_read_handler;

        *taskp = task;
    }

    ctx = task->ctx;

    if (task->event.complete) {
    /*
    该函数一般会进来两次，第一次是通过原始数据发送触发走到这里，这时候complete = 0，第二次是当线程池读取数据完成，则会通过
    ngx_thread_pool_handler->ngx_http_copy_thread_event_handler->ngx_http_request_handler->ngx_http_writer在次走到这里，不过
    这次complete已经在ngx_thread_pool_handler置1
     */   
        task->event.complete = 0;

        if (ctx->err) {
            ngx_log_error(NGX_LOG_CRIT, file->log, ctx->err,
                          "pread() \"%s\" failed", file->name.data);
            return NGX_ERROR;
        }

        return ctx->read;
    }

    ctx->fd = file->fd;
    ctx->buf = buf;
    ctx->size = size;
    ctx->offset = offset;

    //这里添加task->event信息到task中，当task->handler指向完后，通过nginx_notify可以继续通过epoll_wait返回执行task->event
    //客户端过来后如果有缓存存在，则ngx_http_file_cache_aio_read中赋值为ngx_http_cache_thread_handler;  
    //如果是从后端获取的数据，然后发送给客户端，则ngx_output_chain_as_is中赋值未ngx_http_copy_thread_handler
    if (file->thread_handler(task, file) != NGX_OK) {
        return NGX_ERROR;
    }

    return NGX_AGAIN;
}


#if (NGX_HAVE_PREAD)
//在ngx_thread_read把该handler添加到线程池中
static void //ngx_thread_read->ngx_thread_read_handler
ngx_thread_read_handler(void *data, ngx_log_t *log)
{//该函数执行后，会通过ngx_notify执行event.handler = ngx_http_cache_thread_event_handler;
    ngx_thread_read_ctx_t *ctx = data;

    ssize_t  n;

    ngx_log_debug0(NGX_LOG_DEBUG_CORE, log, 0, "thread read handler");

    //缓存文件数据会拷贝到dst中，也就是ngx_output_chain_ctx_t->buf,然后在ngx_output_chain_copy_buf函数外层会重新把ctx->buf赋值给新的chain，然后write出去
    n = pread(ctx->fd, ctx->buf, ctx->size, ctx->offset);

    if (n == -1) {
        ctx->err = ngx_errno;

    } else {
        ctx->read = n;
        ctx->err = 0;
    }

#if 0
    ngx_time_update();
#endif

    ngx_log_debug4(NGX_LOG_DEBUG_CORE, log, 0,
                   "pread read return read size: %z (err: %i) of buf-size%uz offset@%O",
                   n, ctx->err, ctx->size, ctx->offset);
}

#else

#error pread() is required!

#endif

#endif /* NGX_THREADS */


ssize_t
ngx_write_file(ngx_file_t *file, u_char *buf, size_t size, off_t offset)
{
    ssize_t  n, written;

    ngx_log_debug5(NGX_LOG_DEBUG_CORE, file->log, 0,
                   "write to filename:%V,fd: %d, buf:%p, size:%uz, offset:%O", &file->name, file->fd, buf, size, offset);

    written = 0;

#if (NGX_HAVE_PWRITE)

    for ( ;; ) {
        //pwrite() 把缓存区 buf 开头的 count 个字节写入文件描述符 fd offset 偏移位置上。文件偏移没有改变。
        n = pwrite(file->fd, buf + written, size, offset);

        if (n == -1) {
            ngx_log_error(NGX_LOG_CRIT, file->log, ngx_errno,
                          "pwrite() \"%s\" failed", file->name.data);
            return NGX_ERROR;
        }

        file->offset += n;
        written += n;

        if ((size_t) n == size) {
            return written;
        }

        offset += n;
        size -= n;
    }

#else

    if (file->sys_offset != offset) {
        if (lseek(file->fd, offset, SEEK_SET) == -1) {
            ngx_log_error(NGX_LOG_CRIT, file->log, ngx_errno,
                          "lseek() \"%s\" failed", file->name.data);
            return NGX_ERROR;
        }

        file->sys_offset = offset;
    }

    for ( ;; ) {
        n = write(file->fd, buf + written, size);

        if (n == -1) {
            ngx_log_error(NGX_LOG_CRIT, file->log, ngx_errno,
                          "write() \"%s\" failed", file->name.data);
            return NGX_ERROR;
        }

        file->offset += n;
        written += n;

        if ((size_t) n == size) {
            return written;
        }

        size -= n;
    }
#endif
}


ngx_fd_t
ngx_open_tempfile(u_char *name, ngx_uint_t persistent, ngx_uint_t access)
{
    ngx_fd_t  fd;

    fd = open((const char *) name, O_CREAT|O_EXCL|O_RDWR,
              access ? access : 0600);

    if (fd != -1 && !persistent) {
        /*
        unlink函数使文件引用数减一，当引用数为零时，操作系统就删除文件。但若有进程已经打开文件，则只有最后一个引用该文件的文件
        描述符关闭，该文件才会被删除。
          */
        (void) unlink((const char *) name); //如果一个文件名有unlink，则当关闭fd的时候，会删除该文件
    }

    return fd;
}


#define NGX_IOVS  8
/*
如果配置xxx_buffers  XXX_buffer_size指定的空间都用完了，则会把缓存中的数据写入临时文件，然后继续读，读到ngx_event_pipe_write_chain_to_temp_file
后写入临时文件，直到read返回NGX_AGAIN,然后在ngx_event_pipe_write_to_downstream->ngx_output_chain->ngx_output_chain_copy_buf中读取临时文件内容
发送到后端，当数据继续到来，通过epoll read继续循环该流程
*/

/*ngx_http_upstream_init_request->ngx_http_upstream_cache 客户端获取缓存 后端应答回来数据后在ngx_http_upstream_send_response->ngx_http_file_cache_create
中创建临时文件，然后在ngx_event_pipe_write_chain_to_temp_file把读取的后端数据写入临时文件，最后在
ngx_http_upstream_send_response->ngx_http_upstream_process_request->ngx_http_file_cache_update中把临时文件内容rename(相当于mv)到proxy_cache_path指定
的cache目录下面
*/

ssize_t
ngx_write_chain_to_file(ngx_file_t *file, ngx_chain_t *cl, off_t offset,
    ngx_pool_t *pool)
{
    u_char        *prev;
    size_t         size;
    ssize_t        total, n;
    ngx_array_t    vec;
    struct iovec  *iov, iovs[NGX_IOVS];

    /* use pwrite() if there is the only buf in a chain */

    if (cl->next == NULL) { //只有一个buf节点
        return ngx_write_file(file, cl->buf->pos,
                              (size_t) (cl->buf->last - cl->buf->pos),
                              offset);
    }

    total = 0; //本次总共写道文件中的字节数，和cl中所有buf指向的内存空间大小相等

    vec.elts = iovs;
    vec.size = sizeof(struct iovec);
    vec.nalloc = NGX_IOVS;
    vec.pool = pool;

    do {
        prev = NULL;
        iov = NULL;
        size = 0;

        vec.nelts = 0;

        /* create the iovec and coalesce the neighbouring bufs */

        while (cl && vec.nelts < IOV_MAX) { //把cl链中的所有每一个chain节点连接到一个iov中
            if (prev == cl->buf->pos) { //把一个chain链中的所有buf放到一个iov中
                iov->iov_len += cl->buf->last - cl->buf->pos;

            } else {
                iov = ngx_array_push(&vec);
                if (iov == NULL) {
                    return NGX_ERROR;
                }

                iov->iov_base = (void *) cl->buf->pos;
                iov->iov_len = cl->buf->last - cl->buf->pos;
            }

            size += cl->buf->last - cl->buf->pos; //cl为所有数据的长度和
            prev = cl->buf->last;
            cl = cl->next;
        } //如果cl链中的所有chain个数超过了IOV_MAX个，则需要下次继续在后面while (cl);回过来处理

        /* use pwrite() if there is the only iovec buffer */

        if (vec.nelts == 1) {
            iov = vec.elts;

            n = ngx_write_file(file, (u_char *) iov[0].iov_base,
                               iov[0].iov_len, offset);

            if (n == NGX_ERROR) {
                return n;
            }

            return total + n;
        }

        if (file->sys_offset != offset) {
            if (lseek(file->fd, offset, SEEK_SET) == -1) {
                ngx_log_error(NGX_LOG_CRIT, file->log, ngx_errno,
                              "lseek() \"%s\" failed", file->name.data);
                return NGX_ERROR;
            }

            file->sys_offset = offset;
        }

        n = writev(file->fd, vec.elts, vec.nelts);

        if (n == -1) {
            ngx_log_error(NGX_LOG_CRIT, file->log, ngx_errno,
                          "writev() \"%s\" failed", file->name.data);
            return NGX_ERROR;
        }

        if ((size_t) n != size) {
            ngx_log_error(NGX_LOG_CRIT, file->log, 0,
                          "writev() \"%s\" has written only %z of %uz",
                          file->name.data, n, size);
            return NGX_ERROR;
        }

        ngx_log_debug3(NGX_LOG_DEBUG_CORE, file->log, 0,
                       "writev to filename:%V,fd: %d, readsize: %z", &file->name, file->fd, n);

        file->sys_offset += n;
        file->offset += n;
        offset += n;
        total += n;

    } while (cl);//如果cl链中的所有chain个数超过了IOV_MAX个，则需要下次继续在后面while (cl);回过来处理

    return total;
}


ngx_int_t
ngx_set_file_time(u_char *name, ngx_fd_t fd, time_t s)
{
    struct timeval  tv[2];

    tv[0].tv_sec = ngx_time();
    tv[0].tv_usec = 0;
    tv[1].tv_sec = s;
    tv[1].tv_usec = 0;

    if (utimes((char *) name, tv) != -1) {
        return NGX_OK;
    }

    return NGX_ERROR;
}


ngx_int_t
ngx_create_file_mapping(ngx_file_mapping_t *fm)
{
    fm->fd = ngx_open_file(fm->name, NGX_FILE_RDWR, NGX_FILE_TRUNCATE,
                           NGX_FILE_DEFAULT_ACCESS);
    if (fm->fd == NGX_INVALID_FILE) {
        ngx_log_error(NGX_LOG_CRIT, fm->log, ngx_errno,
                      ngx_open_file_n " \"%s\" failed", fm->name);
        return NGX_ERROR;
    }

    if (ftruncate(fm->fd, fm->size) == -1) {
        ngx_log_error(NGX_LOG_CRIT, fm->log, ngx_errno,
                      "ftruncate() \"%s\" failed", fm->name);
        goto failed;
    }

    fm->addr = mmap(NULL, fm->size, PROT_READ|PROT_WRITE, MAP_SHARED,
                    fm->fd, 0);
    if (fm->addr != MAP_FAILED) {
        return NGX_OK;
    }

    ngx_log_error(NGX_LOG_CRIT, fm->log, ngx_errno,
                  "mmap(%uz) \"%s\" failed", fm->size, fm->name);

failed:

    if (ngx_close_file(fm->fd) == NGX_FILE_ERROR) {
        ngx_log_error(NGX_LOG_ALERT, fm->log, ngx_errno,
                      ngx_close_file_n " \"%s\" failed", fm->name);
    }

    return NGX_ERROR;
}


void
ngx_close_file_mapping(ngx_file_mapping_t *fm)
{
    if (munmap(fm->addr, fm->size) == -1) {
        ngx_log_error(NGX_LOG_CRIT, fm->log, ngx_errno,
                      "munmap(%uz) \"%s\" failed", fm->size, fm->name);
    }

    if (ngx_close_file(fm->fd) == NGX_FILE_ERROR) {
        ngx_log_error(NGX_LOG_ALERT, fm->log, ngx_errno,
                      ngx_close_file_n " \"%s\" failed", fm->name);
    }
}


ngx_int_t
ngx_open_dir(ngx_str_t *name, ngx_dir_t *dir)
{
    dir->dir = opendir((const char *) name->data);

    if (dir->dir == NULL) {
        return NGX_ERROR;
    }

    dir->valid_info = 0;

    return NGX_OK;
}


ngx_int_t
ngx_read_dir(ngx_dir_t *dir)
{
    dir->de = readdir(dir->dir);

    if (dir->de) {
#if (NGX_HAVE_D_TYPE)
        dir->type = dir->de->d_type;
#else
        dir->type = 0;
#endif
        return NGX_OK;
    }

    return NGX_ERROR;
}


ngx_int_t
ngx_open_glob(ngx_glob_t *gl)
{
    int  n;

    n = glob((char *) gl->pattern, 0, NULL, &gl->pglob);

    if (n == 0) {
        return NGX_OK;
    }

#ifdef GLOB_NOMATCH

    if (n == GLOB_NOMATCH && gl->test) {
        return NGX_OK;
    }

#endif

    return NGX_ERROR;
}


ngx_int_t
ngx_read_glob(ngx_glob_t *gl, ngx_str_t *name)
{
    size_t  count;

#ifdef GLOB_NOMATCH
    count = (size_t) gl->pglob.gl_pathc;
#else
    count = (size_t) gl->pglob.gl_matchc;
#endif

    if (gl->n < count) {

        name->len = (size_t) ngx_strlen(gl->pglob.gl_pathv[gl->n]);
        name->data = (u_char *) gl->pglob.gl_pathv[gl->n];
        gl->n++;

        return NGX_OK;
    }

    return NGX_DONE;
}


void
ngx_close_glob(ngx_glob_t *gl)
{
    globfree(&gl->pglob);
}











































/*
Linux fcntl函数详解(2011-07-18 20:22:14)转载