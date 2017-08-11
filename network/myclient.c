/*************************************************************************
	> File Name: myclient.c
	> Author: 
	> Mail: 
	> Created Time: 2017年08月11日 星期五 08时16分59秒
 ************************************************************************/
#include<stdio.h>
#include<termios.h>
#include<unistd.h>
#include<assert.h>
#include<string.h>
#include<string.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<cJSON.h>

//向上移动光标 cursor_up(3); 代表向上移动3.
#define cursor_up(n) \
    fprintf(stderr, "\033[%dA", (n))
//向下移动光标
#define cursor_down(n) \
    fprintf(stde
//向右移动光标
#define cursor_right(n) \
    fprintf(stderr, "\033[%dC", (n))
#define START "\t\t\t\t\t************************************************************************************\n"

int getch();
void log_in(int client_fd);
void log_up(int client_fd);
void my_error(char *string,int line);
void log_in_ui();
void find_passwd(int client_fd);
int main()
{
    struct sockaddr_in addr;
    memset(&addr,0,sizeof(addr));
    int client_fd;
    char buf[400];
    memset(buf,0,sizeof(buf));
    addr.sin_family=AF_INET;
    addr.sin_port=htons(4507);
    addr.sin_addr.s_addr=inet_addr("127.0.0.1");//服务器地址

    if((client_fd=socket(AF_INET,SOCK_STREAM,0))<0)//创建客户端套接字
    {
        my_error("client socket",__LINE__);
    }
    if(connect(client_fd,(struct sockaddr *)&addr,sizeof(struct sockaddr))<0)//将套接字绑定到服务器地址
    {
       my_error("connect",__LINE__);
    }

    int choice;
    while(1)
    {
        char buf[20];
        log_in_ui();
        memset(buf,0,sizeof(buf));
        scanf("%d",&choice);
        switch(choice)
        {
            case 1:
                log_in(client_fd);
                break;
            case 2:
                log_up(client_fd);
                break;
            case 3:
                find_passwd(client_fd);
                break;
        }
    }
    close(client_fd);
    printf("thanks for using!\n");
    return 0;
    
}
void log_in_ui()
{   
    system("clear");
    printf(START);
    printf("\t\t\t\t\t\t\t\t欢迎来到聊天室\n");
    printf("请输入您的选项：\n");
    printf("\t\t1、登录\n");
    printf("\t\t2、注册\n");
    printf("\t\t3、找回密码\n");
    printf(START);
}

 void log_up(int client_fd)
 {
    char passwd1[20],passwd2[20],name[100];
    int i=0,count=0;
     while(1)//判断用户名是否重复
    {
        printf("请输入注册账户(不要包含空格):");
        fgets(name,100,stdin);
        name[strlen(name)-1]='\0';
        cJSON * account;
        cJSON_AddStringToObject(account,"name",name);
        char *accountinfo=cJSON_Print(account);
        if(send(client_fd,accountinfo,strlen(accountinfo),0)<0)
        {
            my_error("send failed",__LINE__);
        }
        char buf[20];
        if(recv(client_fd,buf,sizeof(buf),0)<0)
        {
            my_error("recv failed!",__LINE__);
        }
        if(strcmp(buf,"账户不存在!")==0)
            break;
        else 
            printf("账户已存在，请重新输入!\n！");

    }
        printf("请输入8位密码(数字字母均可):");
     while(1)
    {
        if(i!=0)
            printf("请重新输入8位密码:");
        for(i=0;;)
        {
            char t=getch();
            if(t=='\n'||t=='\r')
            {
                passwd1[i]='\0';
                break;
            }
            if(t==127&&i>=1)
            {
                printf("\b \b");
                passwd1[--i]='\0';
            }
            if (t != 127)
           {
                printf("*");
                passwd1[i++] = t;
            }
            if(i<0)
            {
                passwd1[0]='\0';
                i=0;
            } 
        }
        if(i==8)
            break;
        if(i!=8)
            printf("\n输入不是8位，请重新输入!\n");
    }
    i=0;
    printf("\n请验证密码:");
 while(1)
    {
        if(i!=0)
            printf("请再次输入密码:");
        for(i=0;;)
        {
            char t=getch();
            if(t=='\n'||t=='\r')
            {
                passwd2[i]='\0';
                break;
            }
            if(t==127&&i>=1)
            {
                printf("\b \b");
                passwd2[--i]='\0';
            }
            if (t != 127)
           {
                printf("*");
                passwd2[i++] = t;
            }
            if(i<0)
            {
                passwd2[0]='\0';
                i=0;
            } 
        }
        if(i==8&&strcmp(passwd2,passwd1)==0)
            {
                char question[100];
                char answer[100];
                printf("请输入密保问题(不要包含空格):");
                fgets(question,100,stdin);
                question[strlen(question)-1]='\0';
                printf("请输入密保答案(不要包含空格):");
                fgets(answer,100,stdin);

                cJSON * root;
                root=cJSON_CreateObject();
                cJSON_AddStringToObject(root,"name",name);
                cJSON_AddStringToObject(root,"password",passwd1);
                cJSON_AddStringToObject(root,"question",question);
                cJSON_AddStringToObject(root,"answer",answer);
                cJSON_AddStringToObject(root,"type","log_up");
                char *out=cJSON_Print(root);
                if(send(client_fd,out,strlen(out),0)<0)
                {
                    my_error("send failed",__LINE__);
                }
                char buf[20];
                if(recv(client_fd,buf,sizeof(buf),0)<0)
                {
                    my_error("recv failed!",__LINE__);
                }
                if(strcmp(buf,"注册成功!")==0)
                {
                    printf("%s\n",buf);
                    break;
                }
            }
        else 
            printf("\n与第一次输入密码不符合，请重新输入!\n");
    }
}

void log_in(int client_fd)
{
    int i=0;
    char passwd[20];
    int count=0;
    printf("用户名:\n密码:\033[1A");
    char name[100], password[30];
    fgets(name,100,stdin);
    name[strlen(name)]='\0';
    fprintf(stderr,"\033[9C");
    while(1)
    {
        if(i!=0)
            printf("请再次输入密码:");
        for(i=0;;)
        {
            char t=getch();
            if(t=='\n'||t=='\r')
            {
                passwd[i]='\0';
                break;
            }
            if(t==127&&i>=1)
            {
                printf("\b \b");
                passwd[--i]='\0';
            }
            if (t != 127)
           {
                printf("*");
                passwd[i++] = t;
            }
            if(i<0)
            {
                passwd[0]='\0';
                i=0;
            } 
        }
        cJSON * root;
        root=cJSON_CreateObject();
        cJSON_AddStringToObject(root,"name",name);
        cJSON_AddStringToObject(root,"password",password);
        cJSON_AddStringToObject(root,"type","log_in");
        char *out=cJSON_Print(root);
         if(send(client_fd,out,strlen(out),0)<0)
        {
            my_error("send failed",__LINE__);
        }
        char buf[20];
        if(recv(client_fd,buf,sizeof(buf),0)<0)
        {
            my_error("recv failed",__LINE__);
        }
        if(strcmp(buf,"登录成功！")==0)
            {
                printf("\t\t\t\t%s\n",buf);
                sleep(2);
                system("sl");
                system("clear");
                break;
            }
        else
            count++;
        if(count!=3)
            printf("您已输错%d次,还有%d次机会!\n",count,3-count);
        if(count==3)
        {
            printf("您已输错3次！\n");
            exit(1);
        }
    }
}
int getch()
{
    int c=0;
    struct termios org_opts, new_opts;
    int res=0;
    //-----  store old settings -----------
     res=tcgetattr(STDIN_FILENO, &org_opts);
     assert(res==0);
    //---- set new terminal parms --------
    memcpy(&new_opts, &org_opts, sizeof(new_opts));
    new_opts.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ECHOPRT | ECHOKE | ICRNL);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_opts);
    c=getchar();
    //------  restore old settings ---------
    res=tcsetattr(STDIN_FILENO, TCSANOW, &org_opts);
    assert(res==0);
    return c;
}
void find_passwd(int client_fd)
{
    char name[20];
    char question[100];
    char answer[100];
    while(1)
    {
        printf("请输入你要找回的用户名:");
        fgets(name,100,stdin);
        name[strlen(name)-1]='\0';
        printf("请输入密保问题");
        fgets(question,100,stdin);
        question[strlen(question)-1]='\0';
        printf("请输入密保问题:");
        fgets(answer,100,stdin);
        answer[strlen(answer)-1]='\0';

        cJSON * root;
        root=cJSON_CreateObject();
        cJSON_AddStringToObject(root,"name",name);
        cJSON_AddStringToObject(root,"question",question);
        cJSON_AddStringToObject(root,"answer",answer);
        cJSON_AddStringToObject(root,"type","find_passwd");
        char *out=cJSON_Print(root);
        if(send(client_fd,out,strlen(out),0)<0)
        {
            my_error("send failed",__LINE__);
        }
        char buf[100];
        if(recv(client_fd,buf,sizeof(buf),0)<0)
        {
            my_error("recv failed",__LINE__);
        }
        if(strcmp(buf,"输入信息错误")==0)
            printf("您输入的信息错误，请重新输入!\n");

        else 
            break;
    }
    printf("请输入修改后的密码:");
    char passwd[20];
    fgets(passwd,20,stdin);
    passwd[strlen(passwd)-1]='\0';
    cJSON *pass;
    cJSON_AddStringToObject(pass,"password",passwd);
    char * passinfo=cJSON_Print(pass);
    if(send(client_fd,passinfo,strlen(passinfo),0)<0)
    {
        my_error("send failed",__LINE__);
    }
    memset(passwd,0,sizeof(passwd));
    if(recv(client_fd,passwd,sizeof(passwd),0)<0)
    {
        my_error("recv failed",__LINE__);
    }
    if(strcmp(passwd,"修改成功!")==0)
        printf("%s\n",passwd);

}
void my_error(char *string,int line)
{
    fprintf(stderr,"line:%d ",line);
    perror(string);
    exit(1);
}
