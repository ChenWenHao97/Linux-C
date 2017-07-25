/*************************************************************************
	> File Name: wait.c
	> Author: 
	> Mail: 
	> Created Time: 2017年07月25日 星期二 10时42分06秒
 ************************************************************************/

#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<wait.h>
int main()
{
    pid_t pid;
    char *msg;
    int k;
    int exit_code;

    printf("study how to get exit code!\n");
    pid=fork();
    switch(pid)
    {
        case 0:
            msg="child process is running";
            k=5;
            exit_code =37;
            break;
        case -1:
            perror("process creation failed!\n");
            exit(1);
        default:
            exit_code=0;
            break;
    }
    if(pid!=0)
    {
        int stat_val;
        pid_t child_pid;

        child_pid=wait(&stat_val);

        printf("child process has exited,pid=%d\n",child_pid);
        if(WIFEXITED(stat_val))
            printf("child exited with code %d\n",WEXITSTATUS(stat_val));
        else 
            printf("child exited abnormally\n");
    }
    else 
    {
        while(k-->0)
        {
            puts(msg);
            sleep(1);
        }
    }
    exit(exit_code);
}
