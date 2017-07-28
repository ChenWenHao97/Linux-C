#define  _GNU_SOURCE
#include<stdio.h>
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

#define PIPE 1
#define IN 2
#define OUT 4
#define OUTAPP 16

int get_param(char *,char (*str)[256]);
void explain_param(char (*str)[256],int *param,int k);
int getlenth(int start,char *buf);
int getstart(int end,char *buf);
/*int empty(char *str) 
{
    for (int i = 0; i < strlen(str); i++) {
        if (str[i] != ' ')
            return 0;
    }
    return 1;
}*/
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
        int isempty = empty(buf);
        if (isempty)
            continue;
        add_history(buf);//记录历史记录，实现上下左右查看
        k=get_param(buf,str);
        explain_param(str,&param,k);
    }
    exit(0);
}
int getstart(int start,char * buf)//得到非空格下标
{
    start+=1;
    while(buf[start]==' ' && start<=strlen(buf))
        start++;
    printf("start=%d\n",start);

    return start;

}
int getlenth(int start,char *buf)//获得非空字符串长度
{
    int lenth=0;
    for(int i=start+1;i<strlen(buf);i++)
    {
        if(buf[i]!=' ')
        {
            lenth++;
        }
        else
            break;
    }
    printf("lenth=%d\n",lenth);
    return lenth;
}

int get_param(char *buf,char (*str)[256])
{
    int start=0,end=0,k=0;//方法一，判断函数起点和终点的空格问题
    int flag;

    while(end<strlen(buf))
    {
        start=getstart(start,buf);
        int lenth=getlenth(start,buf);
        end=start+lenth;
        printf("end is %d\n",end);
        strncpy(str[k++],buf+start,lenth+1);//用strncpy可以实现从指定起始位置开始复制
        if(strcmp(str[k-1],"\0")==0)
            k--;
        printf("k is =%d\n",k);
        start=end;

    }
}
return k;

    /*
    char st[5000];//方法二
    strcpy(st, buf);
    char bf[5000], gf[5000];
    int i = 0;
    while (sscanf(st,"%s %[^\n]", bf, gf) == 2)//就是将输入分为两部分v
    {
        strcpy(str[i++], bf);
        strcpy(st, gf);
    }
    strcpy(str[i++], st);
    for (int j = strlen(str[i - 1]) - 1; j >= 0; j--)
    {
        if (str[i - 1][j] == ' ')
            str[i - 1][j] = '\0';
        else
            break;
    }
	return k;
    */

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
    int file=0;
    char file2[100][256]={0};
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
            for(int j=i+1;j<len;j++)
                strcpy(file2[file++],str[j]);
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
    char *pipe[256];
    if((*param)&PIPE)//记录管道符号的后半部分
    {
        for(i=0;i<file;i++)
            pipe[i]=file2[i];
        pipe[i]=(char*)NULL;
    }

    for(i=0;i<m;i++)
    {
        argv[i]=store[i];//指针数组就可以存char *NULL
    }
    argv[i]=(char*)NULL;
    if(len==0)//只是回车或者其他字符，直接返回
    {
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
        if((*param)&PIPE)
        {
            fd=open("/tmp/cwhshelltxt",O_RDWR|O_CREAT|O_TRUNC,0644);
            dup2(fd,1);
            execvp(store[0],argv);
            exit(0);
        }

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
        if((*param&PIPE))
        {
            pid_t pid ;
            pid=fork();
            if(pid==0)
            {
                int fd;
                fd=open("/tmp/cwhshelltxt",O_RDONLY);
                dup2(fd,0);
                execvp(pipe[0],pipe);
                exit(0);
            }
            if(pid==-1)
            {
                perror("fork");
                return;
            }
            if(pid>0)
            {
                waitpid(pid,NULL,0);
                remove("/tmp/cwhshelltxt");
                return ;
            }
        }
        return;
    }
}

