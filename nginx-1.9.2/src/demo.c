#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/prctl.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/*
[nginx]# ps -ef | grep nginx
liubin     726  3469  0 20:00 pts/4    00:00:00 ./nginx
liubin     727   726  0 20:00 pts/4    00:00:00 nginx:worker process
liubin     728   726  0 20:00 pts/4    00:00:00 nginx:worker process
liubin     740   726  0 20:00 pts/4    00:00:00 nginx:dispatcher process
liubin     745   726  0 20:00 pts/4    00:00:00 nginx:worker process

[nginx]# kill -9 745
[nginx]# ps -ef | grep nginx
liubin 726 3469 0 20:00 pts/4 00:00:00 ./nginx
liubin 727 726 0 20:00 pts/4 00:00:00 nginx:worker process
liubin 728 726 0 20:00 pts/4 00:00:00 nginx:worker process
liubin 740 726 0 20:00 pts/4 00:00:00 nginx:dispatcher process
liubin 2714 726 0 20:30 pts/4 00:00:00 nginx:worker process

//