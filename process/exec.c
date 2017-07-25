/*************************************************************************
	> File Name: exec.c
	> Author: 
	> Mail: 
	> Created Time: 2017年07月25日 星期二 09时39分12秒
 ************************************************************************/
#include<stdlib.h>
#include<sys/wait.h>
#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
extern char **environ;
int main(int argc,char *argv[])
{
    pid_t pid;
    int stat_val;
    printf("Exec example!\n");
    pid=fork();
    switch(pid)
    {
        case -1:
            perror("process creation failed\n");
            exit(1);
        case 0:
            printf("child process is running\n");
            printf("my pid=%d,parentpid=%d\n",getpid(),getppid());
            printf("uid=%d,gid=%d\n",getuid(),getgid());
            execve("processimage",argv,environ);
            printf("process never go to here~\n");
            exit(0);
        default:
            printf("parent process is running!\n");
            exit(0);
    }
    wait(&stat_val);
    exit(0);
}
