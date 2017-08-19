#include<stdio.h>
#include<unistd.h>

#define cursor_gotoxy(y, x) \
    fprintf(stderr, "\033[%d;%dH", (y), (x))

//向上移动光标 cursor_up(3); 代表向上移动3.
#define cursor_up(n) \
    fprintf(stderr, "\033[%dA", (n))
//向下移动光标
#define cursor_down(n) \
    fprintf(stderr, "\033[%dB", (n))
//向右移动光标
#define cursor_right(n) \
    fprintf(stderr, "\033[%dC", (n))
//向左移动光标
#define cursor_left(n) \
    fprintf(stderr, "\033[%dD", (n))
#define cursor_hide fprintf(stderr, "\033[?25l")
#define cursor_show fprintf(stderr, "\033[?25h")
#define closeattr fprintf(stderr, "\033[0m")
#define highlight fprintf(stderr, "\033[1m")
#define underline fprintf(stderr, "\033[4m")
#define flash fprintf(stderr, "\033[5m")
#define invert fprintf(stderr, "\033[7m")
#define transparent fprintf(stderr, "\033[8m")
#define color(color) \
        fprintf(stderr, "\033[%dm", (color))：
#define clearscreen fprintf(stderr, "\033[2J")
#define clearline fprintf(stderr, "\033[K")

int main()
{
    int n,m;
    printf("username:\npassword:\033[1A");
    char un[30], password[30];
    scanf("%s",un);
    fprintf(stderr,"\033[9C");
    scanf("%s",password);

    printf("your un=%s,pwd=%s\n.",un,password);
}
