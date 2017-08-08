#include<stdio.h>
#include<sys/time.h>
#include<sys/types.h>
#include<unistd.h>
#include<time.h>
#include<sys/select.h>
#include<stdlib.h>

void display_time(const char *string)
{
    int seconds;

    seconds=time((time_t *)NULL);
    printf("%s,%d\n",string,seconds);
}
int main()
{
    fd_set readfds;
    struct timeval timeout;
    int ret;

    FD_ZERO(&readfds);
    FD_SET(0,&readfds);

    while(1)
    {
        timeout.tv_sec=10;//初始化放在里面才是10sec
        timeout.tv_usec=0;
        display_time("before select");//fasfsdafas
        ret=select(1,&readfds,NULL,NULL,&timeout);
        display_time("after select");
        switch(ret)
        {
            case 0:
                printf("No data in ten seconds.\n");
                exit(0);
                break;
            case -1:
                perror("select");
                exit(1);
                break;
            default:
                getchar();
                printf("Data is available now.\n");
        }
    }

}
