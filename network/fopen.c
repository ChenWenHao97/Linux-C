/*************************************************************************
	> File Name: fopen.c
	> Author: 
	> Mail: 
	> Created Time: 2017年08月18日 星期五 18时58分10秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
int main()
{
    FILE *fp;
    FILE *tofp;
    char filename[20];
    char buf[4096];
    char to[4096];
    printf("input：filename");
    scanf(" %s%*c",filename);
    printf("toname ");
    scanf(" %s",to);
    fp=fopen(filename,"r");
    if(fp==NULL)
    {
        printf("打开失败!");
        exit(0);
    }
    else
    {
        tofp=fopen(to,"w+");

        while(fgets(buf,100,fp)!=NULL)
        {
            fputs(buf,tofp);
        }
    }
}
