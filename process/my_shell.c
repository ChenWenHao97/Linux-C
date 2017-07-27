#include<stdio.h>
#define  _GNU_SOURCE
#include<string.h>
#include<pwd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<unistd.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<dirent.h>
#include<readline/readline.h>
#include<readline/history.h>

#define RUNBACK 32
#define PIPE 1
#define IN 2
#define OUT 4
#define INAPP 8
#define OUTAPP 16

void print();
int get_param(char *,char (*str)[256]);
void explain_param(char (*str)[256],int *param,int k);
int main()
{
    char *buf;
    char str[100][256]={0};
    int pid;
    int param=0;
    int k;//二维数组的一维大小
    chdir("/home/cwh");
    while(1)
    {
        param=0;
        char tip[500];
        struct passwd *pwd;
        pwd=getpwuid(getuid());
        sprintf(tip,"%s:%s$ ",pwd->pw_name,get_current_dir_name());
        buf=readline(tip);
        if(strcmp(buf,"exit")==0)
            break;
        add_history(buf);//记录历史记录
        k=get_param(buf,str);
        explain_param(str,&param,k);
    }
    exit(0);
}

int get_param(char *buf,char (*str)[256])
{
    int i;
    int j=0,k=0,q=0;
    int start=0,end=0;
    int flag=0;
    strcat(buf, " ");
    for(i=0;i<strlen(buf);i++)
    {
        flag=0;
        if(buf[i]==' ')
        {
            q=0;
            start=i-1;
            for(j=end;j<=start;j++)
            {
                flag=1;
                str[k][q++]=buf[j];
            }
            for(j=start+1;j<strlen(buf);j++)
            {
                if(buf[j]!=' ')
                {
                    end=j;
                    break;
                }
            }
        }
        if(flag)
        {
            str[k][q]='\0';
            k++;
        }
    }
    return k;
}

void explain_param(char (*str)[256],int *param,int k)
{
    int i;
    char store[100][256];//存ls参数
    char rest[256];
    int back=0;
    int len=k;
    int flag=0;
    int m=0;
    char file1[256],file2[256];
    for(i=0;i<len;i++)
    {
        if(strcmp(str[i],"cd")==0)
        {
            flag=1;
            continue;
        }

        if(strcmp(str[i],">")==0)//一定要++i，不然就会重复读取
        {
            (*param)|=OUT;
            strcpy(rest,str[++i]);
            continue;
        }
        if(strcmp(str[i],">>")==0)
        {
            (*param)|=OUTAPP;
            strcpy(rest,str[++i]);
            continue;
        }
        if(strcmp(str[i],"<")==0)
        {
            (*param)|=IN;
            strcpy(rest,str[++i]);
            continue;
        }
        if(strcmp(str[i],"<<")==0)
        {
            (*param)|=INAPP;
            strcpy(rest,str[i+1]);
            continue;
        }
        if(strcmp(str[i],"|")==0)
        {
            (*param)|=PIPE;
            strcpy(file1,str[i-1]);
            strcpy(file2,str[i+1]);
            break;
        }
        if(strcmp(str[i],"&")==0)
        {
            back=1;
            continue;
        }
        strcpy(store[m++],str[i]);
    }

    char *argv[256];
    for(i=0;i<m;i++)
    {
        argv[i]=store[i];//指针数组就可以存char *NULL
    }
    argv[i]=(char*)NULL;
    if(len==0&&store[0]=='\n')
    {
        printf("cwh@kevin:%s$ \n",get_current_dir_name());
        return;
    }
    pid_t pid;
    if(flag==0||back)
        pid=fork();
    if(back&&pid>0)//有了&而且父进程要结束
    {
        printf("process id %d\n",pid);
        return ;
    }

        if(flag)//如果有cd的情况
        {
            if(len!=1)
            {
                if(chdir(store[0])<0)
                    printf("No such file or directory\n");
            }
            else
                chdir("/home/cwh");
            return ;
        }
    if(pid==0)
    {
        int is_redirect=0;
        int fd;
        if((*param)&OUT)
            fd=open(rest,O_RDWR|O_CREAT|O_TRUNC,0644), is_redirect=1;
        else if((*param)&OUTAPP)
            fd=open(rest,O_RDWR|O_CREAT|O_APPEND,0644),is_redirect=1;
        else if((*param)&IN)
            fd=open(rest,O_RDONLY),is_redirect=2;
        if(is_redirect && fd<0)
        {
            perror("open");
            exit(-1);
        }
        if(is_redirect==1)
            dup2(fd, STDOUT_FILENO);
        if(is_redirect==2)
            dup2(fd,STDIN_FILENO);
        int err=execvp(store[0], argv);
        if (err!=0)
            perror("error:");
        exit(0);
    }
    if(pid == -1)
    {
        printf("creat process failed!");
        return;
    }
    if(pid > 0)
    {
        waitpid(pid,NULL,0);
        return;
    }
}
