/*************************************************************************
	> File Name: errno.c
	> Author: 
	> Mail: 
	> Created Time: 2017年07月19日 星期三 09时41分15秒
 ************************************************************************/

#include<stdio.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<string.h>
#include<errno.h>

int main(void) {
        int i;

    for(i = 0;i < 140;i++) {
                errno = i;
                printf("%d error:%s\n",errno,strerror(errno));
            
    }

        return 0;

}
