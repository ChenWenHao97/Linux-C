/*************************************************************************
	> File Name: unlink_temp.c
	> Author: 
	> Mail: 
	> Created Time: 2017年07月19日 星期三 08时29分59秒
 ************************************************************************/

#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdlib.h>
void my_err(const char * err_string,int line)
{
    fprintf(stderr,"line:%d ",line);
    perror(err_string);
    exit(1);
}
int main()
{
    int fd;
    char buf[32];

    if((fd=open("temp",O_RDWR|O_CREAT|O_TRUNC,S_IRWXU))<0)
        my_err("open",__LINE__);
    if(unlink("temp")<0)
        my_err("unlink",__LINE__);
    printf("file unlinked\n");
//删除了文件，但是进程仍然可以继续使用，fd可以继续进行操作
    if(write(fd,"temp",5)<0)
        my_err("write",__LINE__);
    if((lseek(fd,0,SEEK_SET))==-1)
        my_err("lseek",__LINE__);
    if(read(fd,buf,5)<0)
        my_err("read",__LINE__);
    printf("%s\n",buf);

    return 0;
}
