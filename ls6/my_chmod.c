/*************************************************************************
	> File Name: my_chmod.c
	> Author: 
	> Mail: 
	> Created Time: 2017年07月17日 星期一 11时08分18秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>

int main(int argc,char **argv)
{
    int mode;
    int mode_u;
    int mode_g;
    int mode_o;
    char *path;

    if(argc<3)
    {
        printf("%s <mode num> <target file>\n",argv[0]);
        exit(0);
    }
    mode=atoi(argv[1]);
    if(mode>777||mode<0)
    {
        printf("mode num error\n");
        exit(0);
    }
    mode_u=mode/100;
    mode_g=(mode-(mode_u*100))/10;
    mode_o=mode-(mode_u*100)-(mode_g*10);
    mode=(mode_u*8*8)+mode_g*8+mode_o;
    path=argv[2];
    if(chmod(path,mode)==-1)
    {
        perror("chmod errorr");
        exit(1);
    }
    return 0;

}
