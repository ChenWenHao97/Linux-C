#include <time.h>
#include<stdio.h>
int main()
{
time_t timep;
time(&timep); /*获取time_t类型当前时间*/
/*转换为常见的字符串：Fri Jan 11 17:04:08 2008*/
printf("%s", ctime(&timep));
return 0;
}
