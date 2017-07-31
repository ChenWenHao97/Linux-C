/*************************************************************************
	> File Name: daemon.c
	> Author:
	> Mail:
	> Created Time: 2017年07月24日 星期一 16时48分15秒
 ************************************************************************/
#include<string.h>
#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<signal.h>
#include<sys/param.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<time.h>
#include<syslog.h>
#include<stdlib.h>
int init_daemon(void)
{
    int pid;
    int i;

    signal(SIGTTOU,SIG_IGN);
    signal(SIGTTIN,SIG_IGN);
    signal(SIGTSTP,SIG_IGN);
    signal(SIGHUP,SIG_IGN);

    pid=fork();
    if(pid>0)
    {
        exit(0);
    }
    else if(pid<0)
        return -1;
    setsid();

    pid=fork();
    if(pid>0)
        exit(0);
    else if(pid<0)
        return -1;

    for(i=0;i<NOFILE;close(i++));
    chdir("/");
    umask(0);

    signal(SIGCHLD,SIG_IGN);
    while(1)
    {
        int fd=open("/home/cwh/protect.txt",O_CREAT|O_APPEND|O_RDWR,0644);
        char str[100];
        strcpy(str,"just for test\n");
        write(fd,str,strlen(str));
        sleep(5);
    }
    return 0;
}
int main()
{
    time_t now;
    init_daemon();
}
