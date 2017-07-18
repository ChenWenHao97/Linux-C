/*************************************************************************
	> File Name: test_unmask.c
	> Author: 
	> Mail: 
	> Created Time: 2017年07月18日 星期二 20时31分17秒
 ************************************************************************/

#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<stdlib.h>
int main()
{
    umask(0);//不屏蔽
    if(creat("example_681.test",S_IRWXU|S_IRWXG|S_IRWXO)<0)
    {
        perror("creat");
        exit(1);
    }
    umask(S_IRWXO);
    if(creat("example_682.test",S_IRWXU|S_IRWXG|S_IRWXO)<0)
    {
        perror("creat");
        exit(1);
    }
    return 0;

}
