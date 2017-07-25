/*************************************************************************
	> File Name: getpid.c
	> Author: 
	> Mail: 
	> Created Time: 2017年07月25日 星期二 11时09分13秒
 ************************************************************************/

#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdlib.h>
#include<wait.h>
int main()
{
    int stat_val;
    pid_t pid;
    
    if((pid=fork())==-1)
    {
        printf("fork error!\n");
        exit(1);
    }
    if(pid==0)
        printf("getpid return %d\n",getpid());

    exit(0);
}
