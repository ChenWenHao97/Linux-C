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
#include<time.h>
#include<pthread.h>

#define HOST "127.0.0.1"
#define USER "root"
#define PASSWD "173874"
#define DB "TESTDB"
#define mouth 45077
void my_error(char *string,int line);
int epoll_fd;
void *thread(void);
void send_file(char *buf,int fd);
void log_in(char *buf,int fd);
void log_up(char *buf,int fd);
void find_passwd(char *buf,int fd);
void create_table(char *name);//建立每个人的表
void change_status(char *name,int fd);//改变每个人在线状态
void search_online_friend(char *buf,int fd);
void add_friend(char *buf,int fd);
void friend_ask(char *buf,int fd);
void search_allfriend(char *name,int fd);
void exit_out(char *buf,int fd);
void delete_friend(char *buf,int fd);
void read_message(char *buf,int fd);
void search_chat(char *buf,int fd);
void chat_withfriend(char *buf,int fd);
char * get_time(void);
void chat(char *buf,int fd);
void create_group(char *buf,int fd);
void dissolve_group(char *buf,int fd);
void manage_group(char *buf,int fd);
void group_ask(char *buf,int fd);
void search_groupchat(char *buf,int fd);
void chat_withgroup(char *buf,int fd);
void chat_with(char *buf,int fd);
void display_group(char *buf,int fd);
void read_group_ask(char *buf,int fd);
void readfile(char *buf,int fd);
void downfile(char *buf,int fd);


typedef struct LINK_USER {
    char name[20];
    int status;
    int fd;
    struct LINK_USER *next;
} LINK_USER;
LINK_USER *head=NULL,*p,*q;
MYSQL *mysql;
void init_link();
int main()
{
    struct sockaddr_in serv_addr,client_addr;
    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=INADDR_ANY;
    serv_addr.sin_port=htons(mouth);
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
        nfds=epoll_wait(epoll_fd,events,40,0);
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
                continue;
            }
            if(res==0)//客户端死亡
            {
                close(events[i].data.fd);
                epoll_ctl(epoll_fd,EPOLL_CTL_DEL,events[i].data.fd,&event);
                continue;
            }

            cJSON *root;
            root=cJSON_Parse(buf);
            cJSON * type=cJSON_GetObjectItem(root,"type");
            init_link();//链表初始化
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
                    break;
               case 5:
                    friend_ask(buf,events[i].data.fd);
                    break;
               case 6:
                    search_allfriend(buf,events[i].data.fd);
                    break;
               case 7:
                    delete_friend(buf,events[i].data.fd);
                    break;
               case 8:
                    chat(buf,events[i].data.fd);
                    break;
               case 9:
                    read_message(buf,events[i].data.fd);
                    break;
               case 10:
                    create_group(buf,events[i].data.fd);
                    break;
               case 11:
                    dissolve_group(buf,events[i].data.fd);
                    break;
               case 12:
                    manage_group(buf,events[i].data.fd);
                    break;
               case 13:
                    group_ask(buf,events[i].data.fd);
                    break;
               case 14:
                    chat_with(buf,events[i].data.fd);
                    break;
               case 15:
                    display_group(buf,events[i].data.fd);
                    break;
               case 16:
                    read_group_ask(buf,events[i].data.fd);
                    break;
               case 17:
                    send_file(buf,events[i].data.fd);
                    break;
               case 18:
                    readfile(buf,events[i].data.fd);
                    break;
               case 19:
                    downfile(buf,events[i].data.fd);
                    break;
               case -1:
                    exit_out(buf,events[i].data.fd);
                    break;
               default:
                    break;
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
	    change_status(name->valuestring,fd);//改变在线状态
    }
}
void change_status(char *name,int fd)//改变在线状态
{
    LINK_USER * find;
    find=head;
    while(find!=NULL)
    {
        if(strcmp(find->name,name)==0)
        {
            find->status=1;
            find->fd=fd;
            break;
        }
        find=find->next;
    }
    char result[400];
    sprintf(result,"update account set status=1 where name=\"%s\";",name);
    if(mysql_real_query(mysql,result,strlen(result))!=0)
    {
        my_error("change satus",__LINE__);
    }
    MYSQL_RES *res=mysql_store_result(mysql);
    mysql_free_result(res);
}
void search_online_friend(char *buf,int fd)//查看在线好友
{
    cJSON *out=cJSON_Parse(buf);
    cJSON *name=cJSON_GetObjectItem(out,"name");
    char result[500];
    memset(result,0,sizeof(result));
    strcpy(result,"select name from ");
    strcat(result,name->valuestring);
    strcat(result," where friend=1;");//是朋友再查看
    if(mysql_real_query(mysql,result,strlen(result))!=0)
    {
        my_error("friend name",__LINE__);
    }
    MYSQL_RES * res=mysql_store_result(mysql);
    MYSQL_ROW row;
    while(row=mysql_fetch_row(res))//更新用户好友信息
    {
        LINK_USER *find=head;
        while(find!=NULL)
        {
            if(strcmp(row[0],find->name)==0 && find->status==1)
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
        my_error("search_onlinfinde_friend",__LINE__);
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
    cJSON *root=cJSON_Parse(buf);
    cJSON *fromname=cJSON_GetObjectItem(root,"fromname");
    cJSON *toname=cJSON_GetObjectItem(root,"toname");
    LINK_USER *find=head;
    char result[400];
    memset(result,0,sizeof(result));
    int flag=0;
    while(find!=NULL)
    {
        if(strcmp(find->name,toname->valuestring)==0)//证明用户名存在
        {
            flag=1;
            break;
        }
        find=find->next;
    }
    free(find);
    if(flag)
    {
        memset(result,0,sizeof(result));
        sprintf(result,"insert friend_ask values(\"%s\",\"%s\");",
        fromname->valuestring,toname->valuestring);
        /*strcpy(result,"insert friend_ask values(\"");//记录请求
        strcat(result,fromname->valuestring);
        strcat(result,"\",\"");
        strcat(result,toname->valuestring);
        strcat(result,"\");");*/
        if(mysql_real_query(mysql,result,strlen(result))!=0)
        {
           // my_error("from to failed",__LINE__);
           send(fd,"添加好友失败",30,0);
            return;
        }
        send(fd,"发送成功",20,0);
    }
    else 
        send(fd,"此用户不存在",20,0);
}

void friend_ask(char *buf,int fd)
{
    cJSON *out=cJSON_Parse(buf) ;
    cJSON *casenum=cJSON_GetObjectItem(out,"casenum");
    cJSON *name=cJSON_GetObjectItem(out,"name");
    MYSQL_RES *res;
    cJSON *root;
    cJSON * rows,*row;
    MYSQL_ROW clo;
    char result[400];
    int jdg;
    memset(result,0,sizeof(result));

    switch(casenum->valueint)
    {
        case 1:
            sprintf(result,"select to_name from friend_ask where from_name=\"%s\";",name->valuestring);

            if(mysql_real_query(mysql,result,strlen(result))!=0)
            {
                my_error("case 1 query",__LINE__);
            }
            res=mysql_store_result(mysql);

            jdg=mysql_affected_rows(mysql);
            if(jdg!=0)
            {
                root=cJSON_CreateObject();
                cJSON_AddItemToObject(root,"list",rows=cJSON_CreateArray());
                while(clo=mysql_fetch_row(res))
                {
                    cJSON_AddItemToObject(rows,"name",cJSON_CreateString(clo[0]));
                }
                char *ask=cJSON_Print(root);
                send(fd,ask,strlen(ask),0);
            }
            else 
                send(fd,"您没有发送好友请求",40,0);
            mysql_free_result(res);
            break;
        case 2:  
           {
            int flag1=0,flag2=0,flag3=0;
            sprintf(result,"select from_name from friend_ask where to_name=\"%s\";",name->valuestring);
           // printf("388 %s\n",result);/////
            if(mysql_real_query(mysql,result,strlen(result))!=0)
            {
                my_error("case 2 recv",__LINE__);
            }
            res=mysql_store_result(mysql); 
            root=cJSON_CreateObject();
            cJSON_AddItemToObject(root,"list",rows=cJSON_CreateArray());
            jdg=mysql_affected_rows(mysql);
           // printf("402 jdg %d\n",jdg);/////
            if(jdg!=0)
            {
              //  printf("405  \n");
                MYSQL_ROW clo1;
                while(clo1=mysql_fetch_row(res))
                {
                    cJSON_AddItemToObject(rows,"name",cJSON_CreateString(clo1[0]));
                }
                mysql_free_result(res);     
                char *ask=cJSON_Print(root);
                //printf("410 %s\n",ask);/////
                send(fd,ask,strlen(ask),0);
                memset(result,0,sizeof(result));
                if(recv(fd,result,sizeof(result),0)<0)
                {
                    my_error("recv case 2",__LINE__);
                }
                if(strcmp(result,"quit")==0)
                    break;
                else 
                {
                    cJSON *out=cJSON_Parse(result);
                    cJSON *agree=cJSON_GetObjectItem(out,"agree");
                    cJSON *selfname=cJSON_GetObjectItem(out,"name");
                    memset(result,0,sizeof(result));
                    sprintf(result,"insert into %s values(\"%s\",\"\",0,1,0);",
                    selfname->valuestring,agree->valuestring);//更改接受者状态
                    //fprintf(stderr, "%s",result);////
                    if(mysql_real_query(mysql,result,strlen(result))!=0)
                    {
                        flag1=1;
                        //my_error("change recver",__LINE__);
                    }
                    res=mysql_store_result(mysql);
                    mysql_free_result(res);
                    memset(result,0,sizeof(result));//更改发送者状态
                    sprintf(result,"insert into %s values(\"%s\",\"\",0,1,0);",
                    agree->valuestring,name->valuestring);
                    if(mysql_real_query(mysql,result,strlen(result))!=0)
                    {
                        flag2=1;
                        //my_error("change sender",__LINE__);
                    }
                    res=mysql_store_result(mysql);
                    mysql_free_result(res);
                    memset(result,0,sizeof(result));//删除记录
                    strcpy(result,"delete from friend_ask where from_name=\"");
                    strcat(result,agree->valuestring);
                    strcat(result,"\";");
                   // printf("445 %s",result);
                    if(mysql_real_query(mysql,result,strlen(result))!=0)
                    {
                        flag3=1;
                       // my_error("delet agree",__LINE__);
                    }
                    res=mysql_store_result(mysql);
                    mysql_free_result(res);
                    if(flag1==1||flag2==1||flag3==1)
                        send(fd,"输入信息有误",20,0);
                }
            }
            else 
               {
                    send(fd,"您没有收到好友请求",30,0);
                    mysql_free_result(res);     
               }
              // fprintf(stderr,"521\n");
           }
            break;
        }
}
void search_allfriend(char *buf,int fd)//显示所有好友
{
    char result[400];
    memset(result,0,sizeof(result));
    cJSON *out=cJSON_Parse(buf);
    cJSON *name=cJSON_GetObjectItem(out,"name");
    sprintf(result,"select name from %s ;",name->valuestring);
    if(mysql_real_query(mysql,result,strlen(result))!=0)
    {
        my_error("search_allfriend failed",__LINE__);
    }
    MYSQL_RES *res=mysql_store_result(mysql);
    MYSQL_ROW row;
    int jdg=mysql_affected_rows(mysql);
    if(jdg==0)
        send(fd,"您没有好友",20,0);
    else 
    {
        cJSON *root=cJSON_CreateObject();
        cJSON *rows;
        cJSON_AddItemToObject(root,"list",rows=cJSON_CreateArray());
        while(row=mysql_fetch_row(res))
        {
            cJSON_AddItemToObject(rows,"name",cJSON_CreateString(row[0]));
        }
        char *ask=cJSON_Print(root);
        send(fd,ask,strlen(ask),0);
    }
    mysql_free_result(res);
}

void delete_friend(char *buf,int fd)//删好友
{
    cJSON *root=cJSON_Parse(buf);
    cJSON *selfname=cJSON_GetObjectItem(root,"selfname");
    cJSON * reducename=cJSON_GetObjectItem(root,"reducename");
    char result[400];
    memset(result,0,sizeof(result));
   // printf("  487 %s",reducename->valuestring);
    sprintf(result,"select * from %s where name=\"%s\";",selfname->valuestring,reducename->valuestring);
    //printf("result  488 %s\n",result);
    if(mysql_real_query(mysql,result,strlen(result))!=0)
    {
        my_error("not exit failed",__LINE__);
    }
    MYSQL_RES *res = mysql_store_result(mysql);//必须执行一遍res才可以！！！！继续运行！！！
    mysql_free_result(res);
    int jdg=mysql_affected_rows(mysql);
    //int jdg;
    if(jdg == 0)
        send(fd,"您没有该好友",20,0);
    else 
    {
        int flag1=0,flag2=0;
        memset(result,0,sizeof(result));
        sprintf(result,"delete from %s where name=\"%s\";",selfname->valuestring,reducename->valuestring);
        //printf("%s\n", result);
        //fprintf(stderr,"%s",result);
        int result1;
        if((result1 = mysql_real_query(mysql,result,strlen(result)))!=0)
        {
            //fprintf(stderr, "#%s#", mysql_error(mysql));
           // my_error("delete ",__LINE__);
            flag1=1;
        }
        memset(result,0,sizeof(result));
        sprintf(result,"delete from %s where name=\"%s\";",reducename->valuestring,selfname->valuestring);
        if(mysql_real_query(mysql,result,strlen(result))!=0)
        {
            flag2=1;
            //my_error("delete 2",__LINE__);
        }
        if(flag1==1||flag2==1)
            send(fd,"删除失败",20,0);
        else 
            send(fd,"删除成功",20,0);
    }
}
void read_message(char *buf,int fd)//未读消息
{
    cJSON *root=cJSON_Parse(buf);
    cJSON *fromname=cJSON_GetObjectItem(root,"fromname");
    char result[400];
    memset(result,0,sizeof(result));
    sprintf(result,"select * from %s where message=1",fromname->valuestring);
    if(mysql_real_query(mysql,result,strlen(result))!=0)
    {
        my_error("read_message",__LINE__);
    }
    MYSQL_RES *res=mysql_store_result(mysql);
    int jdg=mysql_affected_rows(mysql);
    if(jdg!=0)
    {
        MYSQL_ROW row,* rows;
        cJSON *out=cJSON_CreateObject();
        cJSON_AddItemToObject(out,"list",rows=cJSON_CreateArray());
        while(row=mysql_fetch_row(res))
        {
            cJSON_AddItemToObject(rows,"name",cJSON_CreateString(row[0]));
        }
        char * sendout=cJSON_Print(out);
        printf("%s542 \n",sendout);
        send(fd,sendout,strlen(sendout),0);
        mysql_free_result(res);
    }
    else 
    {
        send(fd,"没有消息",20,0);
    }
}
   
void chat(char *buf,int fd)
{
    cJSON *root=cJSON_Parse(buf);
    cJSON *casenum=cJSON_GetObjectItem(root,"casenum");
    cJSON *fromname=cJSON_GetObjectItem(root,"fromname");
    cJSON *toname=cJSON_GetObjectItem(root,"toname");
    cJSON *chat=cJSON_GetObjectItem(root,"chat");
    switch(casenum->valueint)
    {
        case 1:
            search_chat(buf,fd);
            break;
        case 2:
            chat_withfriend(buf,fd);
            break;
    }
}

void search_chat(char *buf,int fd)
{
    cJSON *root=cJSON_Parse(buf);
    cJSON *fromname=cJSON_GetObjectItem(root,"fromname");
    cJSON *toname=cJSON_GetObjectItem(root,"toname");
    cJSON *chat=cJSON_GetObjectItem(root,"chat");
    char result[100000];
    memset(result,0,sizeof(result));
    sprintf(result,"select * from chat where (fromname=\"%s\" and toname=\"%s\") or " \
            "(fromname=\"%s\" and toname=\"%s\");",fromname->valuestring,toname->valuestring,
            toname->valuestring,fromname->valuestring);
    if(mysql_real_query(mysql,result,strlen(result))!=0)
    {
        my_error("search chat result",__LINE__);
    }
    MYSQL_RES *res=mysql_store_result(mysql);
    MYSQL_ROW row,*rows,clo;
    int jdg=mysql_affected_rows(mysql);
    if(jdg!=0)
    {
        cJSON *chat_res = cJSON_CreateObject();
        cJSON *chatarr;
        cJSON_AddItemToObject(chat_res, "list", chatarr = cJSON_CreateArray());
        while (clo = mysql_fetch_row(res))
        {
            cJSON *newone;
            cJSON_AddItemToArray(chatarr, newone = cJSON_CreateObject());
            cJSON_AddStringToObject(newone, "chattime", clo[0]);
            cJSON_AddStringToObject(newone, "fromname", clo[1]);
            cJSON_AddStringToObject(newone, "toname", clo[2]);
            cJSON_AddStringToObject(newone, "chat", clo[3]);
        }
        char *sendout=cJSON_Print(chat_res);
        //printf("634   %s\n",sendout);
        mysql_free_result(res);
        send(fd,sendout,strlen(sendout),0);
        char c_message[400];
        memset(c_message,0,sizeof(c_message));
        sprintf(c_message,"update %s set message=0 where name=\"%s\";",fromname->valuestring,toname->valuestring);
        if(mysql_real_query(mysql,c_message,strlen(c_message))!=0)
        {
            //my_error("change message",__LINE__);
            send(fd,"输入信息有误",20,0);
        }
    }
    else 
    {
        send(fd,"没有聊天记录",20,0);
    }
}

void chat_withfriend(char *buf,int fd)
{
    char result[100000];
    cJSON *root=cJSON_Parse(buf);
    cJSON *fromname=cJSON_GetObjectItem(root,"fromname");
    cJSON *toname=cJSON_GetObjectItem(root,"toname");
    cJSON *chat=cJSON_GetObjectItem(root,"chat");

    if(strcmp(buf,"quit")==0)
        return;
    cJSON *root1=cJSON_Parse(buf);
    cJSON * fromname1=cJSON_GetObjectItem(root1,"fromname");
    cJSON * toname1=cJSON_GetObjectItem(root1,"toname");
    cJSON * chat1=cJSON_GetObjectItem(root1,"chat");
    sprintf(result,"insert into chat (fromname,toname,chat) value(\"%s\",\"%s\",\"%s\");",
    fromname1->valuestring,toname1->valuestring,chat1->valuestring);//插入聊天记录总表
    if(mysql_real_query(mysql,result,strlen(result))!=0)
    {
        my_error("chat with",__LINE__);
    }
    MYSQL_RES *one=mysql_store_result(mysql);
    mysql_free_result(one);
    memset(result,0,sizeof(result));
    
    //接受者标记有未读数据message
    sprintf(result,"update %s set message=1 where name=\"%s\";",toname->valuestring,fromname->valuestring);
    if(mysql_real_query(mysql,result,strlen(result))!=0)
    {
        my_error("chat with",__LINE__);
    }
    MYSQL_RES *three=mysql_store_result(mysql);
    mysql_free_result(three);
}

void create_group(char *buf,int fd)
{
    //fprintf(stderr,"\033[33m$$##%s##$$\033[0m\n",buf);
    // sleep(30);
    cJSON *root=cJSON_Parse(buf);
    cJSON *table=cJSON_GetObjectItem(root,"table");
    cJSON * name=cJSON_GetObjectItem(root,"name");
    cJSON *num=cJSON_GetObjectItem(root,"group");
    char result[1000];
    memset(result,0,sizeof(result));
    sprintf(result,"%s",table->valuestring);
   // printf("705  %s \n",result);
    if(mysql_real_query(mysql,result,strlen(result))!=0)
    {
       // fprintf(stderr,"\033[32m$$##%s##$$\033[0m", mysql_error(mysql));
        my_error("create group",__LINE__);
    }
    MYSQL_RES *res=mysql_store_result(mysql);
    mysql_free_result(res);
    // printf("###712\n");
    memset(result,0,sizeof(result));
    sprintf(result,"insert into %s_group values(\"%s\",1,1,0);"
    ,name->valuestring,num->valuestring);
    // printf("713 %s\n",result);
    if(mysql_real_query(mysql,result,strlen(result))!=0)
    {
        // fprintf(stderr,"\033[33m[%s]\033[0m\n",mysql_error(mysql));
        my_error("create group",__LINE__);
    }
    memset(result,0,sizeof(result));
    sprintf(result,"create table %s_member(name varchar(20));",num->valuestring);
    if(mysql_real_query(mysql,result,strlen(result))!=0)//建立成员表
    {
        fprintf(stderr,"\033[33m[%s]\033[0m\n",mysql_error(mysql));  
        my_error("create group",__LINE__);
    }
    memset(result,0,sizeof(result));
    sprintf(result,"insert into %s_member values(\"%s\");",num->valuestring,name->valuestring);
    // printf("result %s\n",result);
    if(mysql_real_query(mysql,result,strlen(result))!=0)//将群住插入成员表
    {
        //fprintf(stderr,"\033[33m[%s]\033[0m\n",mysql_error(mysql));
        my_error("create group",__LINE__);
    }
    //printf("717\n");
    memset(result,0,sizeof(result));
    sprintf(result,"insert into allgroup values (\"%s\");",num->valuestring);
    if(mysql_real_query(mysql,result,strlen(result))!=0)
    {
        my_error("insert allgroup",__LINE__);
    }
}

void dissolve_group(char *buf,int fd)
{
    cJSON *root=cJSON_Parse(buf);
    cJSON *name=cJSON_GetObjectItem(root,"name");
    cJSON *group=cJSON_GetObjectItem(root,"group");
    char result[400];
    memset(result,0,sizeof(result));
    sprintf(result,"select * from %s_group where group_name=\"%s\" and owner=1;"
    ,name->valuestring,group->valuestring);
   // printf("%s \n",result);
    if(mysql_real_query(mysql,result,strlen(result))!=0)
    {
        //fprintf(stderr,"\033[32m$$##%s##$$\033[0m", mysql_error(mysql));       
        my_error("dissolve",__LINE__);
    }
    MYSQL_RES *res=mysql_store_result(mysql);
    int jdg=mysql_affected_rows(mysql);
    if(jdg!=0)
    {
        mysql_free_result(res);
        memset(result,0,sizeof(result));
        memset(result,0,sizeof(result));
        sprintf(result,"drop table %s;",group->valuestring);
        printf("%s\n",result);
        if(mysql_real_query(mysql,result,strlen(result))!=0)
        {
           fprintf(stderr,"\033[32m$$##%s##$$\033[0m", mysql_error(mysql));           
            my_error("dissolve",__LINE__);
        }
        memset(result,0,sizeof(result));
        sprintf(result,"select name from %s_member;",group->valuestring);
        if(mysql_real_query(mysql,result,strlen(result))!=0)
        {
           // fprintf(stderr,"\033[32m$$##%s##$$\033[0m", mysql_error(mysql)); 
            my_error("dissolve",__LINE__);
        }
        MYSQL_RES *res1=mysql_store_result(mysql);
        MYSQL_ROW row;
        while(row=mysql_fetch_row(res1))//把每个人相应的群删除
        {
            memset(result,0,sizeof(result));
            sprintf(result,"delete from %s_group where group_name=\"%s\";",row[0],group->valuestring);
            if(mysql_real_query(mysql,result,strlen(result))!=0)
            {
                //fprintf(stderr,"\033[32m$$##%s##$$\033[0m", mysql_error(mysql));
                
                my_error("dissolve",__LINE__);
            }
        }
        mysql_free_result(res1);
        memset(result,0,sizeof(result));
        sprintf(result,"drop table %s_member",group->valuestring);
        if(mysql_real_query(mysql,result,strlen(result))!=0)
        {
            my_error("drop table member",__LINE__);
        }
        memset(result,0,sizeof(result));
        sprintf(result,"delete  from allgroup where name=\"%s\";",group->valuestring);
        //printf("%s\n",result);
        if(mysql_real_query(mysql,result,strlen(result))!=0)
        {
            my_error("delete record error",__LINE__);
        }
        send(fd,"解散该群成功",50,0);
    }
    else 
        send(fd,"您不能解散该群",50,0);

}

void manage_group(char *buf,int fd)
{
    cJSON *root=cJSON_Parse(buf);
    cJSON *toname=cJSON_GetObjectItem(root,"toname");
    cJSON *casenum=cJSON_GetObjectItem(root,"casenum");
    cJSON *fromname=cJSON_GetObjectItem(root,"fromname");
    cJSON *group=cJSON_GetObjectItem(root,"group");
    switch(casenum->valueint)
    {
        case 1:
        {
            char result[100];
            memset(result,0,sizeof(result));
            sprintf(result,"select * from %s where name=\"%s\";",group->valuestring,toname->valuestring);
            if(mysql_real_query(mysql,result,strlen(result))!=0)
            {
                my_error("name not in group ",__LINE__);
            }
            MYSQL_RES * res=mysql_store_result(mysql);
            int jdg=mysql_affected_rows(mysql);
            mysql_free_result(res);
            if(jdg!=0)
            {
                memset(result,0,sizeof(result));
                sprintf(result,"delete from %s where name=\"%s\";",group->valuestring,toname->valuestring);
                if(mysql_real_query(mysql,result,strlen(result))!=0)
                {
                    my_error("name not in group ",__LINE__);
                }
                send(fd,"此人已经踢出群",20,0);
            }
            else 
                send(fd,"不能删除此人",20,0);
        }
            break;
        case 2:
        {
            char result[100];
            memset(result,0,sizeof(result));
            sprintf(result,"select * from account where name=\"%s\";",toname->valuestring);
            if(mysql_real_query(mysql,result,strlen(result))!=0)
            {
                my_error("name not in group ",__LINE__);
            }
            MYSQL_RES * res=mysql_store_result(mysql);
            int jdg=mysql_affected_rows(mysql);
            mysql_free_result(res);
            if(jdg!=0)
            {
                memset(result,0,sizeof(result));
                sprintf(result,"insert into group_ask values(\"%s\",\"%s\",\"%s\");",
                fromname->valuestring,toname->valuestring,group->valuestring);
                if(mysql_real_query(mysql,result,strlen(result))!=0)
                {
                    my_error("name not in group ",__LINE__);
                }
                send(fd,"发送成功",20,0);
            }
            else 
                send(fd,"发送失败",20,0);
        }
            break;
    }

}

void group_ask(char *buf,int fd)
{
    fprintf(stderr,"\033[32m$#%s#$\033[0m", buf);  
    cJSON *root=cJSON_Parse(buf);
    cJSON *casenum=cJSON_GetObjectItem(root,"casenum");
    switch(casenum->valueint)
    {
        case 1://发送的群邀请
        {
            char result[400];
            cJSON *fromname=cJSON_GetObjectItem(root,"fromname");
            
            memset(result,0,sizeof(result));
            sprintf(result,"select * from group_ask where from_name=\"%s\";",fromname->valuestring);
            if(mysql_real_query(mysql,result,strlen(result))!=0)
            {
                my_error("group ask 1",__LINE__);
            }
            MYSQL_RES *res=mysql_store_result(mysql);

            int jdg=mysql_affected_rows(mysql);
            
            if(jdg!=0)
            {
                MYSQL_ROW row;
                cJSON *group_ask = cJSON_CreateObject();
                cJSON *grouparr;
                cJSON_AddItemToObject(group_ask, "list", grouparr = cJSON_CreateArray());

                while(row=mysql_fetch_row(res))
                {
                    cJSON *newone;
                    cJSON_AddItemToArray(grouparr, newone = cJSON_CreateObject());
                    cJSON_AddStringToObject(newone, "toname", row[1]);
                    cJSON_AddStringToObject(newone, "group", row[2]);
                }
                char *sendout=cJSON_Print(group_ask);
                fprintf(stderr,"\033[32m$#%d#$\033[0m", __LINE__);
                send(fd,sendout,strlen(sendout),0);
            }
            else 
                send(fd,"没有发送群邀请",30,0);
            mysql_free_result(res);
        }        

            break;
        case 2://收到的群邀请
        {
            char result[1000];
            cJSON *tomname=cJSON_GetObjectItem(root,"tomname");
            memset(result,0,sizeof(result));
            sprintf(result,"select * from group_ask where to_name=\"%s\";",tomname->valuestring);
            if(mysql_real_query(mysql,result,strlen(result))!=0)
            {
                my_error("group ask 1",__LINE__);
            }
            MYSQL_RES *res=mysql_store_result(mysql);
            int jdg=mysql_affected_rows(mysql);
            if(jdg!=0)
            {
                MYSQL_ROW row;
                cJSON *group_ask = cJSON_CreateObject();
                cJSON *grouparr;
                cJSON_AddItemToObject(group_ask, "list", grouparr = cJSON_CreateArray());
                while(row=mysql_fetch_row(res))
                {
                    cJSON *newone;
                    cJSON_AddItemToArray(grouparr, newone = cJSON_CreateObject());
                    cJSON_AddStringToObject(newone, "fromname", row[0]);
                    cJSON_AddStringToObject(newone, "group", row[2]);
                } 
                char *sendout=cJSON_Print(group_ask);
                send(fd,sendout,strlen(sendout),0);
                while(1)
                {
                    memset(result,0,sizeof(result));
                    if(recv(fd,result,sizeof(result),0)<0)
                    {
                        my_error("group_recv",__LINE__);
                    }
                    if(strcmp(result,"quit")==0)
                        break;
                    //printf("932 %s",result);
                    cJSON *recvout=cJSON_Parse(result);
                    cJSON *agree=cJSON_GetObjectItem(recvout,"agree");
                    cJSON *name=cJSON_GetObjectItem(recvout,"name");
                    memset(result,0,sizeof(result));
                    sprintf(result,"insert into %s_group values(\"%s\",0,1,0);",
                    name->valuestring,agree->valuestring);
                    // printf("939 %s",result);
                    if(mysql_real_query(mysql,result,strlen(result))!=0)
                    {
                        my_error("group ask 2",__LINE__);
                    }
                    memset(result,0,sizeof(result));
                    sprintf(result,"delete from group_ask where to_name=\"%s\";",name->valuestring);
                    if(mysql_real_query(mysql,result,strlen(result))!=0)
                    {
                        my_error("group ask 2",__LINE__);
                    }
                    memset(result,0,sizeof(result));
                    sprintf(result,"insert into %s_member values(\"%s\");",agree->valuestring,name->valuestring);
                    if(mysql_real_query(mysql,result,strlen(result))!=0)
                    {
                        fprintf(stderr,"\033[32m$$##%s##$$\033[0m", mysql_error(mysql)); 
                        my_error("group ask 2",__LINE__);
                    }
                }
              
            }
            else 
                send(fd,"没有收到群邀请",30,0);
            
            mysql_free_result(res);
        }
            break;
    }
}

void chat_with(char *buf,int fd)
{
    cJSON *root=cJSON_Parse(buf);
    cJSON *casenum=cJSON_GetObjectItem(root,"casenum");
    switch(casenum->valueint)
    {
        case 1:
            search_groupchat(buf,fd);
            break;
        case 2:
        fprintf(stderr,"\033[33%d#\033[0m\n",__LINE__);
            chat_withgroup(buf,fd);
            break;
    }

}

void search_groupchat(char *buf,int fd)
{
    cJSON *root=cJSON_Parse(buf);
    cJSON *group=cJSON_GetObjectItem(root,"group");
    cJSON *name=cJSON_GetObjectItem(root,"name");
    char result[10000];
    memset(result,0,sizeof(result));
    sprintf(result,"select * from allgroup;");
    if(mysql_real_query(mysql,result,strlen(result))!=0)
    {
        printf("1075 %s\n",result);
        send(fd,"输入群号有误",20,0);
        //printf("1066\n");
        return;
    }
    MYSQL_RES *rest = mysql_store_result(mysql);
    mysql_free_result(rest);
    memset(result,0,sizeof(result));
    sprintf(result,"select * from %s;",group->valuestring);
    printf("result %s\n",result);
    if(mysql_real_query(mysql,result,strlen(result))!=0)
    {
        fprintf(stderr,"[break:%d]{%s}",__LINE__,mysql_error(mysql));
        send(fd,"没有聊天记录",30,0);
        //my_error("search chat",__LINE__);
        return;
    }
    MYSQL_RES *res=mysql_store_result(mysql);
    MYSQL_ROW * row,*rows,*clo;
    int jdg=mysql_affected_rows(mysql);
    if(jdg==0)
    {
        fprintf(stderr,"[break:%d]",__LINE__);
        send(fd,"没有聊天记录",30,0);
        return;
    }
    else 
    {
        cJSON *sendout=cJSON_CreateObject();//查询聊天记录
        cJSON_AddItemToObject(sendout,"list",rows=cJSON_CreateArray());
        while(clo=mysql_fetch_row(res))
        {
            cJSON *arr=cJSON_CreateObject();
            cJSON_AddStringToObject(arr,"chattime",clo[0]);
            cJSON_AddStringToObject(arr,"name",clo[1]);
            cJSON_AddStringToObject(arr,"chat",clo[2]);
            cJSON_AddItemToArray(rows,arr);
        }       
        //fprintf(stderr,"\033[33%d#\033[0m\n",__LINE__);
        
        char *sendtoclient=cJSON_Print(sendout);
        send(fd,sendtoclient,strlen(sendtoclient),0);
        mysql_free_result(res);
        
        memset(result,0,sizeof(result));//更新提醒
        sprintf(result,"update %s_group set read_message=1 where group_name=\"%s\";",name->valuestring,
        group->valuestring);
        // fprintf(stderr,"\033[33m$$%s$$\033[0m\n",result);
        if(mysql_real_query(mysql,result,strlen(result))!=0)
        {
            // fprintf(stderr,"\033[33m$$%s$$\033[0m\n", mysql_error(mysql));
            my_error("search chat",__LINE__);
        }        
    }
}

void chat_withgroup(char *buf,int fd)
{
    int flag1=0,flag2=0;
    cJSON *root=cJSON_Parse(buf);
    cJSON *group=cJSON_GetObjectItem(root,"group");
    cJSON *chat=cJSON_GetObjectItem(root,"chat");
    cJSON *name=cJSON_GetObjectItem(root,"name");
    
    char result[1000];
    memset(result,0,sizeof(result));
    sprintf(result,"select * from allgroup;");
    if(mysql_real_query(mysql,result,strlen(result))!=0)
    {
        flag1=1;
        send(fd,"该群不存在",20,0);
        //mysql_free_result(res);
        return;
    }
    MYSQL_RES *res = mysql_store_result(mysql);
    mysql_free_result(res);
    memset(result,0,sizeof(result));
    sprintf(result,"insert into %s (name,chat) values(\"%s\",\"%s\");",group->valuestring,
    name->valuestring,chat->valuestring);
    printf("result 1147 %s\n",result);
    if(mysql_real_query(mysql,result,strlen(result))!=0)
    {
        fprintf(stderr,"\033[33m[%s]\033[0m", mysql_error(mysql));
        my_error("search chat",__LINE__);
    }
}

void display_group(char *buf,int fd)
{
    cJSON *root=cJSON_Parse(buf);
    cJSON *name=cJSON_GetObjectItem(root,"name");
    char result[1000];
    memset(result,0,sizeof(result));
    sprintf(result,"select * from %s_group where status=1;",name->valuestring);
    //printf("1058%s\n",result);
    if(mysql_real_query(mysql,result,strlen(result))!=0)
    {
        my_error("display ",__LINE__);
    }
    //printf("1063\n");
    MYSQL_RES *res=mysql_store_result(mysql);
    MYSQL_ROW *row,*rows,clo;
    int jdg=mysql_affected_rows(mysql);
    //printf("1067 %d\n",jdg);
    if(jdg==0)
    {
        send(fd,"没有加入任何群",30,0);
        return;
    }
    //printf("1073\n");
    cJSON *sendout=cJSON_CreateObject();
    cJSON_AddItemToObject(sendout,"list",rows=cJSON_CreateArray());
    while(clo=mysql_fetch_row(res))
    {
        cJSON_AddItemToArray(rows,row=cJSON_CreateObject());
        cJSON_AddStringToObject(row,"name",clo[0]);
    }
    char *sendtoclient=cJSON_Print(sendout);
    //printf("%s \n",sendtoclient);
    send(fd,sendtoclient,strlen(sendtoclient),0);
    mysql_free_result(res);

}
void read_group_ask(char *buf,int fd)
{
    // printf("1098 \n");
    cJSON *root=cJSON_Parse(buf);
    cJSON *name=cJSON_GetObjectItem(root,"name");
    char result[1000];
    memset(result,0,sizeof(result));
    sprintf(result,"select * from group_ask where to_name=\"%s\";",name->valuestring);
    //printf("%s\n",result);
    if(mysql_real_query(mysql,result,strlen(result))!=0)
    {
        //my_error("read_group",__LINE__);
        send(fd,"没有收到群请求",30,0);
        return;
    }
    MYSQL_RES *res=mysql_store_result(mysql);
    MYSQL_ROW row;
    cJSON *rows,*arr;
    int jdg=mysql_affected_rows(mysql);
    if(jdg==0)
    {
        send(fd,"没有收到群请求",30,0);
        return;
    }
    else 
    {
        // printf("1119\n");
        cJSON *sendout=cJSON_CreateObject();
        cJSON_AddItemToObject(sendout,"list",rows=cJSON_CreateArray());
        while(row=mysql_fetch_row(res))
        {
            cJSON_AddItemToArray(rows,arr=cJSON_CreateObject());
            cJSON_AddStringToObject(arr,"name",row[0]);
        }
        char *sendto=cJSON_Print(sendout);
       // printf("%s\n",sendto);
        send(fd,sendto,strlen(sendto),0);
    }
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
    res=mysql_store_result(mysql);
    int jdg=mysql_affected_rows(mysql);
    if(jdg==0)
    {
        memset(result,0,sizeof(result));
        sprintf(result, "insert into account value(\"%s\",\"%s\",\"%s\",\"%s\",0);",
                name->valuestring, password->valuestring, question->valuestring,
                answer->valuestring);
        if(mysql_real_query(mysql,result,sizeof(result))!=0)//将用户写入数据库
        {
            send(fd,"注册失败!",20,0);
            //my_error("log_up query failed",__LINE__);
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
    strcat(result,"( name varchar(20) NULL , chat text ,status int ,friend int NULL,message int NULL," 
    "UNIQUE(name))DEFAULT CHARSET=utf8;");
    //名字不能重复
    if(mysql_real_query(mysql,result,sizeof(result))!=0)
    {
        my_error("create self table",__LINE__); 
    }
    memset(result,0,sizeof(result));
    //创建群列表
    sprintf(result,"create table %s_group ( group_name varchar(20) unique,owner int,status int,read_message"
    " int)DEFAULT CHARSET=utf8;",name);
    printf("964 %s",result);
    if(mysql_real_query(mysql,result,sizeof(result))!=0)
    {
        my_error("create self table",__LINE__); 
    }
    // printf("##969 \n##");
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
    sprintf(result,"select * from account where name=\"%s\",and question=\"%s\",and answer=\"%s\";",
    name->valuestring,question->valuestring,answer->valuestring);
    if(mysql_real_query(mysql,result,strlen(result))!=0)
    {
        my_error("case 1 recv",__LINE__);
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

void send_file(char *buf,int fd)
{
    fprintf(stderr,"here\n");
    cJSON *root=cJSON_Parse(buf);
    cJSON *name=cJSON_GetObjectItem(root,"name");
    cJSON *size=cJSON_GetObjectItem(root,"size");
    cJSON *friend=cJSON_GetObjectItem(root,"friend");
    cJSON *file=cJSON_GetObjectItem(root,"fname");

    int sendfd;
    FILE *fp;
    int flag;
    int sum=0;
    char result[1002];
    memset(result,0,sizeof(result));
    sprintf(result,"insert into filetable(name,fname,friend) values(\"%s\",\"%s\",\"%s\");",
                    name->valuestring,file->valuestring,friend->valuestring);
    //printf("#%s#\n",result);

    cJSON *cans=cJSON_CreateObject();
    int ret=0;

    if(mysql_real_query(mysql,result,strlen(result))!=0)
    {
        //fprintf(stderr,"\033[33m%s\033[0m\n",mysql_error(mysql));
        printf("文件重名\n");
        ret++;
    }
    memset(result,0,sizeof(result));
    
    sprintf(result,"select * from filetable where fname=\"%s\";",
            file->valuestring);
    //fprintf(stderr,"\033[32m#%s#\033[0m\n",result);
    if(mysql_real_query(mysql,result,strlen(result))!=0)
    {
        fprintf(stderr,"\033[33m%s\033[0m\n",mysql_error(mysql));       
        //printf("1305 错误\n");
        ret++;
    }
    
    char id[1000]={0};
    MYSQL_RES *res=mysql_store_result(mysql);
    MYSQL_ROW row;
    while(row=mysql_fetch_row(res))
    {
        strcpy(id,row[0]);
    }
    mysql_free_result(res);
    char bufp[256];
    //fprintf(stderr,"%s",id);
    if ((fp=fopen(id,"w+")) == NULL)
    {
        ret++;
    }
    
    cJSON_AddNumberToObject(cans,"res",ret);
    cJSON_AddStringToObject(cans,"fid",id);
    char *sendresult=cJSON_Print(cans);
    //fprintf(stderr,"\033[33m[%s]\033[0m\n",sendresult);       
    
    send(fd,sendresult,strlen(sendresult),0);
    if (ret!=0) {
        if (fp!=NULL)
            fclose(fp);
        return;
    }
        
    while(1)
    {
        if(sum>=size->valueint)
            break;
        memset(bufp,0,sizeof(bufp));
        int needread=(size->valueint-sum>=256)?256:size->valueint-sum;        
    
        sum+=recv(fd,bufp,needread,0);//返回值是大小
            //perror("recv");
        fwrite(bufp,needread,1,fp);
        
        if(sum>=size->valueint)
            break;
    }
    fclose(fp);
}


void readfile(char *buf,int fd)
{
    char result[100];
    memset(result,0,sizeof(result));
    cJSON *root=cJSON_Parse(buf);
    cJSON *friend=cJSON_GetObjectItem(root,"friend");
    sprintf(result,"select * from filetable where friend=\"%s\";",friend->valuestring);
    if(mysql_real_query(mysql,result,strlen(result))!=0)
    {
        my_error("select file",__LINE__);
    }
    int jdg=mysql_affected_rows(mysql);
    //fprintf(stderr,"\033[33m[%d]\033[0m\n",__LINE__);
    if(jdg==0)
    {
        send(fd,"没有人给您发文件",40,0);
        return;
    }
    MYSQL_RES *res=mysql_store_result(mysql);
    MYSQL_ROW row;
    MYSQL *arr,*list;
    cJSON *sendout=cJSON_CreateObject();
    cJSON_AddItemToObject(sendout,"list",arr=cJSON_CreateArray());
    while(row=mysql_fetch_row(res))
    {
        cJSON *newone;
        cJSON_AddItemToArray(arr,newone=cJSON_CreateObject());
        cJSON_AddStringToObject(newone,"fname",row[2]);
        cJSON_AddStringToObject(newone,"name",row[1]);
    }
    char *read=cJSON_Print(sendout);
    send(fd,read,strlen(read),0);
    mysql_free_result(res);
}

void downfile(char *buf,int fd)
{
    cJSON *root=cJSON_Parse(buf);
    cJSON *friend=cJSON_GetObjectItem(root,"friend");
    cJSON *filename=cJSON_GetObjectItem(root,"filename");
    char result[100];
    memset(result,0,sizeof(result));
    sprintf(result,"select * from filetable where friend=\"%s\" and fname=\"%s\";"
    ,friend->valuestring,filename->valuestring);
    if(mysql_real_query(mysql,result,strlen(result))!=0)
    {
        fprintf(stderr,"\033[33m%s\033[0m\n",mysql_error(mysql));
    }
    MYSQL_RES *res=mysql_store_result(mysql);
    int jdg=mysql_affected_rows(mysql);
    if(jdg==0)
    {
        send(fd,"输入信息有误",30,0);
        mysql_free_result(res);
        return;
    }
    else
    {
        send(fd,"输入信息正确",30,0);
        MYSQL_ROW row;
        FILE *fp;
        char idname[100];
        while(row=mysql_fetch_row(res))
            strcpy(idname,row[0]);
        fp=fopen(idname,"r");
        fseek(fp, 0L, SEEK_END); 

        int size = ftell(fp);
        char buf[256];
        fseek(fp, 0L, SEEK_SET);
        send(fd,(void*)&size,sizeof(size),0);
        perror("send");

        int sum=0;
        char yes[100];
        if(recv(fd,yes,sizeof(yes),0)<0)
        {
            my_error("recv yes",__LINE__);
        }
        
        if(strcmp(yes,"no")==0)
            return;
        while(1)
        {
            if(sum>=size)
                break;
            memset(buf,0,sizeof(buf));
            
            int needread=(size-sum>=256)?256:size-sum;
            fread(buf,needread,1,fp);
            sum+=send(fd,buf,needread,0);
            
            if(sum>=size)
                break;
        }
        remove(idname);
        mysql_free_result(res);
    }
    memset(result,0,sizeof(result));
    sprintf(result,"delete from filetable where fname=\"%s\";",filename->valuestring);
    if(mysql_real_query(mysql,result,strlen(result))!=0)
    {
        fprintf(stderr,"\033[33m%s\033[0m\n",mysql_error(mysql));
    }
    
}
void my_error(char *string,int line)
{
    fprintf(stderr,"line:%d",line);
    perror(string);
    exit(1);
}
void exit_out(char *buf,int fd)
{
    cJSON *root=cJSON_Parse(buf);
    cJSON *name=cJSON_GetObjectItem(root,"name");
    char result[400];
    sprintf(result,"update account set status=0 where name=\"%s\";",name->valuestring);
    if(mysql_real_query(mysql,result,strlen(result))!=0)
    {
        my_error("exit ",__LINE__);
    }
    LINK_USER *find=head;
    while(find!=NULL)
    {
        if(strcmp(find->name,name->valuestring)==0)
        {
            find->status=0;
            break;
        }
        find=find->next;
    }

}
void init_link()//链表初始化
{
    char result[400];
    memset(result,0,sizeof(result));
    strcpy(result,"select name from account;");
    if(mysql_real_query(mysql,result,strlen(result))!=0)
    {
        //printf("初始化错误\n");
        //        my_error("init link",__LINE__);
    }
    MYSQL_RES *res=mysql_store_result(mysql);
    MYSQL_ROW row;

    while(row=mysql_fetch_row(res))
    {
        p=(LINK_USER *)malloc(sizeof(LINK_USER));
        strcpy(p->name,row[0]);
        p->status=0;
        if(head==NULL)
            head=p;
        else 
            q->next=p;
        q=p;
    }
    mysql_free_result(res);
    /*LINK_USER *find=head;
    while(find!=NULL)
    {
        printf("name %s\n",find->name);
        find=find->next;
    }*/
}
char * get_time(void)
{
    time_t timep;
    time(&timep); /*获取time_t类型当前时间*/
    /*转换为常见的字符串：Fri Jan 11 17:04:08 2008*/
    return ctime(&timep);
}
