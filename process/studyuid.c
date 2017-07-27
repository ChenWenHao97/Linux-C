/*************************************************************************
	> File Name: studyuid.c
	> Author: 
	> Mail: 
	> Created Time: 2017年07月25日 星期二 19时15分47秒
 ************************************************************************/

#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<errno.h>
#include<stdlib.h>
#include<error.h>
#include<string.h>
extern int erron;

int main()
{
    int fd;

    printf("uid study:\n");
    printf("process's uid =%d,euid =%d\n",getuid(),geteuid());

    if((fd=open("test.c",O_RDWR))==-1)
    {
        printf("open failture,errno is %d :%s \n",errno,strerror(errno));
        exit(1);
    }
    else 
        printf("Open successfully!\n");

    close(fd);
    exit(0);
}
