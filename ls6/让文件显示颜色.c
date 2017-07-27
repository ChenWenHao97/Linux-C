/*************************************************************************
	> File Name: color.c
	> Author: 
	> Mail: 
	> Created Time: 2017年07月21日 星期五 09时39分38秒
 ************************************************************************/

#include<stdio.h>
#define BLACK  "\033[30m"
#define ORANGE  "\033[31m"
#define GREEN  "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE  "\033[34m"
#define PINK  "\033[35m"
#define WHITE  "\033[37m"
       
#define SKY "\033[36m"
#define CANCEL "\033[0m"
int main() {
    printf(ORANGE "%s\n" CANCEL,"orange");//链接
    printf(SKY "%s\n" CANCEL,"sky");//文件
    printf(BLUE "%s\n"CANCEL,"blue");//目录
    printf(GREEN "%s\n"CANCEL,"green");//字符

    printf(PINK "%s\n"CANCEL,"pupple");//块
    printf(WHITE "%s\n"CANCEL,"white");//FIFO
    printf(BLACK"%s\n"CANCEL,"blcak");//socket
    return 0;
}
