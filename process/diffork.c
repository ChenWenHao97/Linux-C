/*************************************************************************
	> File Name: diffork.c
	> Author:
	> Mail:
	> Created Time: 2017年07月24日 星期一 14时58分34秒
 ************************************************************************/

#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdlib.h>
int globVar=5;

int main()
{
    pid_t pid;
    int var =1,i;
    pid=fork();
    //pid=vfork();
    switch(pid)
    {
        case 0:
            i=3;
        while(i-->0)
        {
            printf("child process is running\n");
            globVar++;
            var++;
            sleep(1);
        }
        printf("child's globVar=%d,var=%d\n",globVar,var);
      //  exit(0);
        return 0;
        case -1:
            perror("process failed\n");
        exit(0);
        default:
        i=5;
        printf("###%d\n",var);
        while(i-->0)
        {
            printf("parent process is running\n");
            globVar++;
            var++;
            sleep(1);
        }
        printf("parent's globVar=%d,var=%d\n",globVar,var);
        exit(0);
    }
    return 0;
}
