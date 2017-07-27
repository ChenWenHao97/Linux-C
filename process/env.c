/*************************************************************************
	> File Name: env.c
	> Author:
	> Mail:
	> Created Time: 2017年07月25日 星期二 09时24分31秒
 ************************************************************************/

#include<stdio.h>
#include<malloc.h>

extern char **environ;

int main(int argc,char *argv[])
{
    int i ;
    printf("argument:\n");
    for(i=0;i<argc;i++)
        printf("argv[%d] is %s\n",i,argv[i]);
    printf("environment:\n");
    for(i=0;environ[i]!=NULL;i++)
        printf("%s\n",environ[i]);
    return 0;
}
