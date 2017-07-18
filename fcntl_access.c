/*************************************************************************
	> File Name: fcntl_access.c
	> Author: 
	> Mail: 
	> Created Time: 2017年07月18日 星期二 08时23分02秒
 ************************************************************************/

#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<stdlib.h>

void my_err(const char *err_string,int line)
{
    fprintf(stderr,"line:%d ",line);
    perror(err_string);
    exit(1);
}

int main()
{
    int ret;
    int access_mode;
    int fd;

    if((fd=open("example_64",O_CREAT|O_TRUNC|O_RDWR,S_IRWXU))==-1)
        my_err("open",__LINE__);
    if((ret=fcntl(fd,F_SETFL,O_APPEND))<0)//设置文件打开方式
        my_err("fcntl",__LINE__);

    if((ret=fcntl(fd,F_GETFL,0))<0)//获取文件打开方式
        my_err("fcntl",__LINE__);
    access_mode=ret & O_ACCMODE;//O_ACCMODE为3，&可以获得ret后两位
    if(access_mode==O_RDONLY)
        printf("example_64 access_mode:read only");
    else if(access_mode==O_WRONLY)
        printf("example_64 access_mode:write only");
    else if(access_mode==O_RDWR)
        printf("example_64 access_mode:write+read");

    if(ret&O_APPEND)
        printf(" ,append");
    printf("\n%d\n",O_APPEND);

    printf("\n%d\n",O_NONBLOCK);

    printf("\n%d\n",O_SYNC);
    if(ret&O_NONBLOCK)
        printf(",nonblock");
    if(ret&O_SYNC)
        printf(" ,sync");

    putchar('\n');

    return 0;
}
