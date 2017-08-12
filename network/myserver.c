/*************************************************************************
	> File Name: myserver.c
	> Author: 
	> Mail: 
	> Created Time: 2017年08月11日 星期五 15时16分48秒
 ************************************************************************/
#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<string.h>
#include<sys/epoll.h>
#include<unistd.h>
#include<cJSON.h>
#include<mysql.h>
#include<assert.h>
#include<pthread.h>

#define HOST "127.0.0.1"
#define USER "root"
#define PASSWD "173874"
#define DB "TESTDB"
 
void my_error(char *string,int line);
int epoll_fd;
void *thread(void);
void log_in(char *buf,int fd);
void log_up(char *buf,int fd);
void find_passwd(char *buf,int fd);
MYSQL *mysql;


int main()
{
    struct sockaddr_in serv_addr,client_addr;
    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=INADDR_ANY;
    serv_addr.sin_port=htons(4507);
    int serv_sockfd,client_sockfd;

    if((serv_sockfd=socket(AF_INET,SOCK_STREAM,0))<0)//套接字创建
    {
        my_error("socket failed",__LINE__);
    }
    if(bind(serv_sockfd,(struct sockaddr *)&serv_addr,sizeof(struct sockaddr_in))<0)//绑定端口
    {
        my_error("bind failed",__LINE__);
    }
    if(listen(serv_sockfd,5)<0)//监听
    {
        my_error("listen",__LINE__);
    }

    mysql=mysql_init(NULL);
    if(mysql_real_connect(mysql,HOST,USER,PASSWD,DB,0,NULL,0)==NULL)
    {
        my_error("mysql connect",__LINE__);
    }
    epoll_fd=epoll_create(40);
    pthread_t pid;
    if(pthread_create(&pid,NULL,(void *)thread,NULL)!=0)//线程创建
    {
        my_error("pthread_create failed",__LINE__);
    }
    if(epoll_fd==-1)
    {
        my_error("epoll failed",__LINE__);
    }
    while(1)//添加事件
    {
        int conn_fd;
        struct sockaddr_in cli_addr;
        int len=sizeof(struct sockaddr_in);
        if((conn_fd=accept(serv_sockfd,(struct sockaddr *)&cli_addr,&len))<0)
        {
            my_error("accept failed",__LINE__);
        }
        struct epoll_event ev;
        ev.events=EPOLLIN;
        ev.data.fd=conn_fd;
        if(epoll_ctl(epoll_fd,EPOLL_CTL_ADD,conn_fd,&ev)==-1)
        {
            my_error("epoll_ctl failed",__LINE__);
        }
        printf("accept client :%s\n",inet_ntoa(cli_addr.sin_addr));
        
    }
    return 0;
}
void *thread(void )
{
   struct epoll_event events[40];
   struct epoll_event event;
   int nfds;//发生事件个数
   char buf[400];
    while(1)//处理事件
    {
        nfds=epoll_wait(epoll_fd,events,40,-1);
        if(nfds==-1)
        {
            my_error("epoll_wait failed",__LINE__);
        }
        for(int i=0;i<nfds;i++)
        {
            memset(buf,0,sizeof(buf));
            int res=recv(events[i].data.fd,buf,sizeof(buf),0);
            if(res<0)
            {
                my_error("client recv",__LINE__);
            }
            if(res==0)//客户端死亡
            {
                epoll_ctl(epoll_fd,EPOLL_CTL_DEL,events[i].data.fd,&event);
                continue;
            }
        
            cJSON *root;
            root=cJSON_Parse(buf);
            cJSON * type=cJSON_GetObjectItem(root,"type");
            switch(type->valueint)
            {
               case 0:
                   log_in(buf,events[i].data.fd);
                   break;
              /* case 1:
                   log_up(buf,events[i].data.fd);
                   break;
               case 2:
                   find_passwd(buf,events[i].data.fd);
                   break;*/
            }
        }
    }
}
void log_in(char *buf,int fd)
{
    cJSON *out;
    out=cJSON_Parse(buf);
    cJSON *name=cJSON_GetObjectItem(out,"name");
    cJSON *password=cJSON_GetObjectItem(out,"password");
    int clo;
    MYSQL_ROW row;
    char result[100];
    memset(result,0,sizeof(result));
    strcpy(result,"select * from account where name=\"");
    strcat(result,name->valuestring);
    strcat(result,"\"");
    strcat(result," and password=\"");
    strcat(result,password->valuestring);
    strcat(result,"\";");
    MYSQL_RES * res; 
    if( mysql_real_query(mysql,result,strlen(result))!=0)
    {
        my_error("mysql query failed",__LINE__);
    }
    res=mysql_store_result(mysql);
    if(res==NULL)
        send(fd,"错误",20,0);
    else 
        send(fd,"登录成功!",20,0);

}
void my_error(char *string,int line)
{
    fprintf(stderr,"line:%d",line);
    perror(string);
    exit(1);
}