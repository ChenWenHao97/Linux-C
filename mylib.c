#include"mylib.h"
#include<stdio.h>
void welcome()
{
    printf("welcome to mylib\n");
}
void outstring(char *str)
{
    if(str!=NULL)
        printf("%s\n",str);
}
