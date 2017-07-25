/*************************************************************************
	> File Name: processimage.c
	> Author: 
	> Mail: 
	> Created Time: 2017年07月25日 星期二 09时35分30秒
 ************************************************************************/

#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>

int main(int argc,char *argv[],char ** envion)
{
    int i;
    printf("i am a process image!\n");
    printf("my pid =%d,parentpid =%d\n",getpid(),getppid());
    printf("uid=%d,gid=%d\n",getuid(),getgid());

    for(i=0;i<argc;i++)
        printf("argv[%d]:%s\n",i,argv[i]);

    return 0;
}

