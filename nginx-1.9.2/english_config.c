/*

Example Configuration

user www www;
worker_processes 2;

error_log /var/log/nginx-error.log info;

events {
    use kqueue;
    worker_connections 2048;
}

...

Directives
Syntax:  accept_mutex on | off;
 
Default:  accept_mutex on; 
Context:  events
 

If accept_mutex is enabled, worker processes will accept new connections by turn. Otherwise, all worker processes will be notified about new connections, and if volume of new connections is low, some of the worker processes may just waste system resources. 

Syntax:  accept_mutex_delay time;
 
Default:  accept_mutex_delay 500ms; 
Context:  events
 

If accept_mutex is enabled, specifies the maximum time during which a worker process will try to restart accepting new connections if another worker process is currently accepting new connections. 

Syntax:  daemon on | off;
 
Default:  daemon on; 
Context:  main
 

Determines whether nginx should become a daemon. Mainly used during development. 

Syntax:  debug_connection address | CIDR | unix:;
 
Default:  