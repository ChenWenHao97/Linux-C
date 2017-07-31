/*************************************************************************
	> File Name: fork3.c
	> Author:
	> Mail:
	> Created Time: 2017年07月24日 星期一 11时35分26秒
 ************************************************************************/

#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdlib.h>

int main()
{
    pid_t pid;

    pid=fork();
    switch(pid)
    {
        case 0:
            while(1)
        {
            printf("A background process,PID:%d\n,parrent ID:%d\n",getpid(),getppid());
            sleep(3);
        }
        case -1:
            perror("process failed!\n");
            exit(-1);
        default:
            printf("i am parent process,my pid is %d\n",getpid());
            exit(0);
    }
}
