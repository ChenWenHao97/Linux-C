/*************************************************************************
	> File Name: get_current.c
	> Author: 
	> Mail: 
	> Created Time: 2017年07月23日 星期日 12时26分13秒
 ************************************************************************/

#include<stdio.h>
#include <stdio.h>
#include <unistd.h>
#include<stdlib.h>
#define  _GNU_SOURCE
int main(int argc,char *argv[])
{
            char *path;
            path = get_current_dir_name();
            printf("%s>\n",path);
            exit(0);

}

/*用gcc -E将预处理的代码导出来查看，如果宏定义的位置不正确。
导出的代码中不会包含get_current_dir_name()的函数原型，自然编译就认为它的返回值是默认的整数，从而导致一个警告。
把宏定义放在前面之后，gcc -E导出的代码中已经包含了正确的函数原型，警告就不会出现了。*/
