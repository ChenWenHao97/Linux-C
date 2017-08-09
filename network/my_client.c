/*************************************************************************
	> File Name: my_client.c
	> Author: 
	> Mail: 
	> Created Time: 2017年08月08日 星期二 17时25分21秒
 ************************************************************************/
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
int main()
{
    struct sockaddr_in addr;
    memset(&addr,0,sizeof(addr));
    int client_fd;
    char buf[40];
    memset(buf,0,sizeof(buf));
    addr.sin_family=AF_INET;
    addr.sin_port=htons(4507);
    addr.sin_addr.s_addr=inet_addr("127.0.0.1");//服务器地址
    if((client_fd=socket(AF_INET,SOCK_STREAM,0))<0)//创建客户端套接字
    {
        perror("client socket");
        exit(1);
    }
    if(connect(client_fd,(struct sockaddr *)&addr,sizeof(struct sockaddr))<0)//将套接字绑定到服务器地址
    {
       perror("connect");
       exit(1);
    }
    while(1)
    {
        printf("please input info:");
        scanf(" %s",buf);
        if(strcmp(buf,"quit")==0)
            break;
        if(send(client_fd,buf,sizeof(buf),0)<0)
        {
            perror("senf faild!");
            exit(1);
        }
        if(recv(client_fd,buf,sizeof(buf),0)<0)
        {
            perror("recv failed!");
            exit(1);
        }
        printf("you receive :%s\n",buf);
    }
    close(client_fd);
    printf("thanks for using!\n");
    return 0;
}

