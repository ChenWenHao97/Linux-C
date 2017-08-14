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
void create_table(char *name);//建立每个人的表
void change_status(char *name);//改变每个人在线状态
void search_online_friend(char *buf,int fd);
/*void add_friend(char *name,int fd);
void friend_ask(char *name,int fd);
void search_friend(char *name,int fd);
void search_group(char *name,int fd);*/
typedef struct LINK_USER {
    char name[20];
    int status;
    struct LINK_USER *next;
} LINK_USER;
LINK_USER *head=NULL,*p,*q;
MYSQL *mysql;

int main()
{
    struct sockaddr_in serv_addr,client_addr;
    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=INADDR_ANY;
    serv_addr.sin_port=htons(45077);
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
               case 1:
                   log_up(buf,events[i].data.fd);
                   break;
               case 2:
                   find_passwd(buf,events[i].data.fd);
                   break;
               case 3:
                    search_online_friend(buf,events[i].data.fd);
                    break;
               case 4:
                    add_friend(buf,events[i].data.fd );
                    break;/*
               case 5:
                    friend_ask(buf,events[i].data.fd);
                    break;
               case 6:
                    search_friend(buf,events[i].data.fd);
                    break;
               case 7:
                    search_group(buf,events[i].data.fd);
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
    int jdg=mysql_affected_rows(mysql);
    if(jdg==0)
    {
        send(fd,"错误",20,0);
        mysql_free_result(res);
    }
    else 
    {
        send(fd,"登录成功!",20,0);
        mysql_free_result(res);
	    change_status(name->valuestring);//改变在线状态
    }
}
void change_status(char *name)//改变在线状态
{
    LINK_USER * find;
    find=head;
    while(find!=NULL)
    {
        if(strcmp(find->name,name)==0)
        {
            find->status=1;
            break;
        }
        find=find->next;
    }
    free(find);
}
void search_online_friend(char *buf,int fd)//查看在线好友
{
    cJSON *out=cJSON_Parse(buf);
    cJSON *name=cJSON_GetObjectItem(out,"name");
    char result[500];
    memset(result,0,sizeof(result));
    strcpy(result,"select name from ");
    strcat(result,name->valuestring);
    strcat(result,"where friend=1;");//是朋友再查看
    if(mysql_real_query(mysql,result,strlen(result))!=0)
    {
        my_error("friend name",__LINE__);
    }
    MYSQL_RES * res=mysql_store_result(mysql);
    MYSQL_ROW row;
    while(row=mysql_fetch_row(res))//更新用户好友信息
    {
        LINK_USER *find=head;
        while(find)
        {
            if(strcmp(row[0],find->name)==0&&find->status==1)
            {
                char status[100];
                memset(status,0,sizeof(status));
                strcpy(status,"update ");
                strcat(status,name->valuestring);
                strcat(status," set status =1 where name=\"");
                strcat(status,row[0]);
                strcat(status,"\";");
                if(mysql_real_query(mysql,status,strlen(status))!=0)
                {
                    my_error("update stauts",__LINE__);
                }
                break;
            }
            find=find->next;
        }
    }
    memset(result,0,sizeof(result));
    strcpy(result,"select name from ");
    strcat(result,name->valuestring);
    strcat(result," where status=1;");
    if(mysql_real_query(mysql,result,strlen(result))!=0)
    {
        my_error("search_online_friend",__LINE__);
    }
    res=mysql_store_result(mysql);
    int jdg=mysql_affected_rows(mysql);
    if(jdg==0)
    {
        send(fd,"没有好友在线",20,0);
    }
    else 
    {
        //保存在二维数组
        MYSQL_ROW row;
        cJSON * root=cJSON_CreateObject();
        cJSON *arr=cJSON_CreateArray();
        cJSON *new;
        cJSON_AddItemToArray(arr,new=cJSON_CreateObject());
        while(row=mysql_fetch_row(res))
        {
            cJSON_AddItemToObject(new,"name",cJSON_CreateString(row[0]));
        }
        cJSON_AddItemToObject(root,"list",arr);
        char *out=cJSON_Print(root);
        send(fd,out,strlen(out),0);
    }
    mysql_free_result(res);
}
void add_friend(char *buf,int fd)
{
    system("clear");
    printf("\t\t\t\t请输入您要添加好友的姓名:");
    char addname[20];
    scanf(" %s",name);

}

void log_up(char *buf,int fd)
{
    MYSQL_ROW row;
    MYSQL_RES * res;
    char result[500];
    cJSON *out=cJSON_Parse(buf);
    cJSON *name=cJSON_GetObjectItem(out,"name");
    cJSON *password=cJSON_GetObjectItem(out,"password");
    cJSON *question=cJSON_GetObjectItem(out,"question");
    cJSON *answer=cJSON_GetObjectItem(out,"answer");
    memset(result,0,sizeof(result));
    strcpy(result,"select * from account where name=\"");
    strcat(result,name->valuestring);
    strcat(result,"\";");
    if(mysql_real_query(mysql,result,sizeof(result))!=0)
    {
        my_error("log_up search failed",__LINE__);
    }
    p=(LINK_USER *)malloc(sizeof(LINK_USER));
    strcpy(p->name,name->valuestring);
    if(head==NULL)//建立链表
        head=p;
    else 
        q->next=p;
    q=p;

    res=mysql_store_result(mysql);
    int jdg=mysql_affected_rows(mysql);
    if(jdg==0)
    {
        memset(result,0,sizeof(result));
        strcpy(result,"insert into account value(\"");//注意格式问题
        strcat(result,name->valuestring);
        strcat(result,"\",\"");
        strcat(result,password->valuestring);
        strcat(result,"\",\"");
        strcat(result,question->valuestring);
        strcat(result,"\",\"");
        strcat(result,answer->valuestring);
        strcat(result,"\");");
        
        if(mysql_real_query(mysql,result,sizeof(result))!=0)
        {
            my_error("log_up query failed",__LINE__);
        }
        send(fd,"注册成功!",20,0);
        create_table(name->valuestring);//开始建立每个人一张表
    }
    else 
        send(fd,"账号已存在!",20,0);
    mysql_free_result(res);
}
void create_table(char *name)//建立表格
{
	char result[500];
	memset(result,0,sizeof(result));
    strcpy(result,"create table ");
    strcat(result,name);
    strcat(result,"( name varchar(20) NULL , chat text NULL,status int NULL,friend int NULL,UNIQUE(name));");
    //名字不能重复
    if(mysql_real_query(mysql,result,sizeof(result))!=0)
    {
        my_error("create self table",__LINE__);
    }
}

void find_passwd(char *buf,int fd)
{
    cJSON *root=cJSON_Parse(buf);
    cJSON *name=cJSON_GetObjectItem(root,"name");
    cJSON *password=cJSON_GetObjectItem(root,"password");
    cJSON *question=cJSON_GetObjectItem(root,"question");
    cJSON *answer=cJSON_GetObjectItem(root,"answer");
    char result[400];

    memset(result,0,sizeof(buf));
    strcpy(result,"select * from account where name=\"");
    strcat(result,name->valuestring);
    strcat(result,"\"and question=\"");
    strcat(result,question->valuestring);
    strcat(result,"\"and answer=\"");
    strcat(result,answer->valuestring);
    strcat(result,"\";");
    if(mysql_real_query(mysql,result,sizeof(result))!=0)
    {
        my_error("query failed",__LINE__);
    }
    MYSQL_RES * res;
    res=mysql_store_result(mysql);
    int jdg=mysql_affected_rows(mysql);
    if(jdg==0)
        send(fd,"输入信息有误",20,0);
    else 
    {
        memset(result,0,sizeof(result));
        strcpy(result,"update account set password=\"");
        strcat(result,password->valuestring);
        strcat(result,"\" where name=\"");
        strcat(result,name->valuestring);
        strcat(result,"\";");
        if(mysql_real_query(mysql,result,sizeof(result))!=0)
        {
            my_error("query failed",__LINE__);
        }
        send(fd,"修改成功!",20,0);
    }
    mysql_free_result(res);
}

void my_error(char *string,int line)
{
    fprintf(stderr,"line:%d",line);
    perror(string);
    exit(1);
}
