/*************************************************************************
	> File Name: my_server.c
	> Author: 
	> Mail: 
	> Created Time: 2017年08月08日 星期二 22时19分07秒
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
#define HOST "127.0.0.1"
#define USER "root"
#define PASSWD "173874"
#define DB "TESTDB"
int main()
{
    struct sockaddr_in serv_addr,client_addr;
    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=INADDR_ANY;
    serv_addr.sin_port=htons(4507);
    char buf[400];
    int serv_sockfd;
    int client_sockfd;

    if((serv_sockfd=socket(AF_INET,SOCK_STREAM,0))<0)//服务器套接字
    {
        perror("socket failed");
        exit(1);
    }
    if(bind(serv_sockfd,(struct sockaddr *)&serv_addr,sizeof(struct sockaddr_in))<0)
    //将套接字绑定到服务器的网络地址上
    {
        perror("bind failed");
        exit(1);
    }
    if(listen(serv_sockfd,5)<0)//监听
    {
        perror("listen");
        exit(1);
    }

    int epoll_fd;
    epoll_fd=epoll_create(40);//创建epoll
    if(epoll_fd==-1)
    {
        perror("epoll failed");
        exit(1);
    }
    struct epoll_event ev;//epoll事件结构体
    struct epoll_event events[40];
    ev.events=EPOLLIN;
    ev.data.fd=serv_sockfd;
    int nfds;//epoll发生个数
    if(epoll_ctl(epoll_fd,EPOLL_CTL_ADD,serv_sockfd,&ev)==-1)
        //注册服务器监听事件
    {
        perror("epoll failed");
        exit(1);
    }

    while(1)
    {
        nfds=epoll_wait(epoll_fd,events,40,1000);//等待事件发生，时间微秒
        if(nfds==-1)
        {
            perror("epoll_wait failed");
            exit(1);
        }
        /*if(nfds==0)//如果没有接受到事件
        {
            printf("anybody!\n");

        }*/
            for(int i=0;i<nfds;i++)
                {
                    if(events[i].data.fd==serv_sockfd)
                    {
                        int len=sizeof(struct sockaddr_in);
                        if((client_sockfd=accept(serv_sockfd,(struct sockaddr*)&client_addr,&len))<0)
                        {
                            //接受客户端的信息
                            perror("accept failed");
                            exit(1);
                        }
                        printf("there is a new user!\n");

                        //向epoll注册客户端监听事件
                        ev.events=EPOLLIN;//可以读取
                        ev.data.fd=client_sockfd;
                        if(epoll_ctl(epoll_fd,EPOLL_CTL_ADD,client_sockfd,&ev)==-1)
                        {
                            perror("client epoll_ctl failed");
                            exit(1);
                        }
                        printf("accept client :%s\n",inet_ntoa(client_addr.sin_addr));  
                    }
                    else
                    {
                        memset(buf,0,sizeof(buf));
                        int rv = recv(events[i].data.fd, buf, sizeof(buf), 0);
                        if(rv<0)
                        {
                            perror("client recv failed");
                            exit(1);
                        }
                        if (rv == 0)
                        {
                            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd,&ev);
                            continue;//删除死亡客户端，防止服务器崩溃
                        }
                        cJSON * root;
                        root=cJSON_Parse(buf);
                        
                        cJSON *name = cJSON_GetObjectItem(root,"name");
                        cJSON *password=cJSON_GetObjectItem(root,"password");
                        
                        MYSQL *mysql=mysql_init(NULL);
                        if(mysql==NULL)
                        {
                            printf("mysql_init failed,line:%d\n",__LINE__);
                            exit(1);

                        }
                        if(!mysql_real_connect(mysql,HOST,USER,PASSWD,DB,0,NULL,0))
                        {
                            //连接确定的数据库，可以不在用use 。。。
                            printf("failed to connect,line:%d",__LINE__);
                            exit(1);
                        }
                        else
                            printf("connct database successful!\n");

                        //if(!mysql_query(mysql,"use TESTDB"))//change
                        //    printf("change successful!\n");
                        //else 
                        //    printf("change database failed.line:%d",__LINE__);
                        
                       /* char insert[256];//write
                        memset(insert,0,sizeof(insert));//insert
                        strcpy(insert,"insert into test (name,password) values ('");
                        strcat(insert,name->valuestring);
                        strcat(insert,"','");//注意‘号
                        strcat(insert,password->valuestring);
                        strcat(insert,"');");
                        if(!mysql_real_query(mysql,insert,strlen(insert)))
                            printf("插入数据成功！");
                        else 
                            printf("insert error,line:%d",__LINE__);*/
                        

                        MYSQL_RES *res;//line
                        MYSQL_ROW row;//colu
                        char search[256];
                        memset(search,0,sizeof(search));
                        strcpy(search,"select * from account where name=\"");
                        strcat(search,name->valuestring);
                        strcat(search,"\" and ");
                        strcat(search,"password=\"");
                        strcat(search,password->valuestring);
                        strcat(search,"\";");
                        printf("%s \n",search);
                        if(!mysql_real_query(mysql,search,strlen(search)))
                            printf("登陆成功！\n");
                        else
                            {
                                printf("登录失败！\n");
                                exit(1);
                            }
                        /*int t;
                        if((t=mysql_query(mysql,"select * from test"))!=0)
                            printf("failed to search,line:%d",__LINE__);
                        res=mysql_store
_result(mysql);
                        printf("show tables:\n");
                        while(row=mysql_fetch_row(res))
                        {
                            for(t=0;t<mysql_num_fields(res);t++)
                                printf("%s\t",row[t]);
                            putchar('\n');
                        }*/
    //                        mysql_free_result(res);
                        send(events[i].data.fd,"I have received your info",40,0);
                    }

            
            }
    }
}
