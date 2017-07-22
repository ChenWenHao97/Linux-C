/*************************************************************************
	> File Name: my_ls.c
	> Author: 
	> Mail: 
	> Created Time: 2017年07月20日 星期四 09时50分40秒
 ************************************************************************/
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdlib.h>
#include<time.h>
#include<sys/stat.h>
#include<unistd.h>
#include<sys/types.h>
#include<limits.h>
#include<dirent.h>
#include<grp.h>
#include<pwd.h>
#include<errno.h>

#define PARM_NONE 0
#define PARM_A 1//将参数定义好
#define PARM_L 2
#define PARM_R 4
#define MAXLEN 80

int maxfilename=0;
int  filename=0;
char arr[256][256];
typedef struct tag{
    char name[1000];
    struct tag *next;
}LINK;

void swap(char *a, char *b);
int PART(char (*arr)[256],int p,int q);
void QUICK(char (*arr)[256],int p,int q);//快排!!!
void my_err(char *err_string,int line);
void display_attribute(struct stat buf,char *name,char*);
void display(int flag_param,char *pathname);//传二维数组
void display_color(char *name);

int main(int argc,char *argv[])
{
    int i,j,k=0;
    char param[10]={0};//存-a，-l
    int flag_param=PARM_NONE;//默认是没有
    struct stat buf;
    char name[20]={0};
    int count=0;//计数-
    int flag = 1;

    for(i=1;i<argc;i++)
    {
        if(argv[i][0]=='-')//证明有参数
        {
            for(j=1;j<strlen(argv[i]);j++)
            {
                param[k++]=argv[i][j];
                count++;
            }
        }
    }
    for(i=0;i<count;i++)//权限问题
    {
        if(param[i]=='a')
            flag_param|=PARM_A;
        else if(param[i]=='l')
            flag_param|=PARM_L;
        else if(param[i]=='R')
            flag_param|=PARM_R;
        else
            break;
    }
   for(i=1;i<argc;i++)
    {
        if(argv[i][0]!='-')
            flag=0;//没有-的，证明就是有目录
    }
//判断后面有没有参数
    if (flag==1)//有参数，没目录情况
    {
        strcpy(name,".");
        display(flag_param,name);
        return 0;
    }
     i=1;
    while(i<argc)
    {
        if(argv[i][0]=='-')
        {
            i++;
            continue;
        }
        else
             strcpy(name,argv[i]);
        if(stat(name,&buf)==-1)//获取属性
            my_err("lstat",__LINE__);
        display(flag_param,name);
        i++;
            
    }
    return 0;
}
void swap(char *a, char *b)
{
    char t[256];
    strcpy(t,a);
    strcpy(a,b);
    strcpy(b,t);
}
int PART(char (*arr)[256],int p,int q)
{
    int i,j;
    j=p-1;
    for(i=p;i<q;i++)
    {
        if(strcmp(arr[i], arr[q]) < 0)//if(arr[i][0]<arr[q][0])
        {
            j++;
            swap(arr[i],arr[j]);
        }
    }
    swap(arr[j+1],arr[q]);
    return j+1;
}

void QUICK(char (*arr)[256],int p,int q)//快排!!!
{
    int r;
    if(p<q)
    {
        r=PART(arr,p,q);
        QUICK(arr,p,r-1);
        QUICK(arr,r+1,q);
    }
}

void my_err(char *err_string,int line)
{
    fprintf(stderr,"line:%d ",line);
    perror(err_string);
    exit(1);
}

void display_attribute(struct stat buf,char *name,char *pathname)
{
    struct passwd *user;
    struct group *group;
    struct stat bu;

    char *curdir = getcwd(NULL, 0);
    //获得现在的工作目录，用于还原，这里getcwd是通过malloc开的空间，所以要记得free.
    chdir(pathname);//切换工作目录
    if(lstat(name,&bu)==-1)
        {
            my_err("stat",__LINE__);
        }
    if(S_ISLNK(bu.st_mode))//判断文件是什么类型
        putchar('l');
    else if(S_ISDIR(bu.st_mode))
        putchar('d');
    else if(S_ISCHR(bu.st_mode))
        putchar('c');
    else if(S_ISBLK(bu.st_mode))
        putchar('b');
    else if(S_ISFIFO(bu.st_mode))
        putchar('f');
    else if(S_ISSOCK(bu.st_mode))
        putchar('s');
    else
        putchar('-');

    //判断user权限
    if(bu.st_mode & S_IREAD)
        putchar('r');
    else 
        putchar('-');
    if(bu.st_mode & S_IWRITE)
        putchar('w');
    else
        putchar('-');
    if(bu.st_mode & S_IEXEC)
        putchar('x');
    else 
        putchar('-');

    //判断group权限
    if(bu.st_mode & S_IRGRP)
        putchar('r');
    else
        putchar('-');
     if(bu.st_mode & S_IWGRP)
        putchar('w');
    else
        putchar('-');
     if(bu.st_mode & S_IXGRP)
        putchar('x');
    else 
        putchar('-');

    //判断others权限
    if(bu.st_mode & S_IROTH)
        putchar('r');
    else
        putchar('-');
    if(bu.st_mode & S_IWOTH)
        putchar('w');
    else
        putchar('-');
     if(bu.st_mode & S_IXOTH)
        putchar('x');
    else
        putchar('-');

    user=getpwuid(bu.st_uid);//获取userID
    group=getgrgid(bu.st_gid);//获取groupID

    printf("%4d",bu.st_nlink);//链接数
    printf("%5s",user->pw_name);//user;
    printf("%5s",group->gr_name);//group;
    printf("%6d",bu.st_size);//文件大小
    char time_buf[255];
    strcpy(time_buf, ctime(&bu.st_atime));
    time_buf[strlen(time_buf)-1] = '\0';//ctime会有一个换行
    printf("%  s",time_buf);//文件时间
    if(name[0]!='\0')
        printf(" %s\n",name);
    
    chdir(curdir);//还原原本的工作目录
    free(curdir);//释放掉空间
}

void display(int flag_param,char *pathname)//传二维数组,pahtname全路径
{
    DIR *dir;
    struct dirent *ptr;
    int count=0;
    struct stat buf;
    int i,j=0;
    char alldir[800][1000];
    char t[200];
    LINK *p,*q,*head=NULL;
    char *pwd;
    int k;
    
    if(stat(pathname,&buf)==-1)
    {
        my_err("lstat",__LINE__);
    }

    switch(flag_param)
    {
        case PARM_NONE:
                    if(S_ISDIR(buf.st_mode))
                    
                    { 
                        count=0;
                        if((dir=opendir(pathname))==NULL)
                            my_err("opendir",__LINE__);
                        while((ptr=readdir(dir))!=NULL)////要排序
                        {
                            if(ptr->d_name[0]!='.')
                                strcpy(arr[count++],ptr->d_name);
                        }
                        QUICK(arr,0,count-1);
                        int k=0;
                        for(i=0;i<count;i++)
                        {
                            printf("%s  ",arr[i]);
                        
                            k++;
                            if(k%5==0)
                                putchar('\n');
                        }
                        if(k%5!=0)
                            putchar('\n');
                        closedir(dir);
                  }
                else 
                     printf("%s\n",pathname);
        
        break;
        
        case PARM_A: 
                count=0;
                if((dir=opendir(pathname))==NULL)
                    my_err("opendir",__LINE__);
                while((ptr=readdir(dir))!=NULL)////要排序
                        strcpy(arr[count++],ptr->d_name);

                    QUICK(arr,0,count-1);
                    int k=0;
                    for(i=0;i<count;i++)
                    {
                        printf("%s ",arr[i]);
                        k++;
                        if(k%5==0)
                            putchar('\n');
                    }
                    if(k%5!=0)
                        putchar('\n');
                    closedir(dir);
            break;

        case PARM_L:
                count=0;
                    if(S_ISDIR(buf.st_mode))
                    {
                        if((dir=opendir(pathname))==NULL)
                            my_err("opendir",__LINE__);
                        while((ptr=readdir(dir))!=NULL)////要排序
                            strcpy(arr[count++],ptr->d_name);
                        QUICK(arr,0,count-1);
                        for(i=0;i<count;i++)
                        {
                            if(arr[i][0]!='.')
                                display_attribute(buf,arr[i],pathname);
                        }
                         closedir(dir);
                    }
                    else
                        display_attribute(buf,".",pathname);
        break;

        case PARM_L+PARM_A:
               count=0;
               if(S_ISDIR(buf.st_mode))
               {
                if((dir=opendir(pathname))==NULL)
                    my_err("opendir",__LINE__);
                while((ptr=readdir(dir))!=NULL)////要排序
                        strcpy(arr[count++],ptr->d_name);
                QUICK(arr,0,count-1);
                    for(i=0;i<count;i++)
                        display_attribute(buf,arr[i],pathname);
               }
               else 
                    display_attribute(buf,".",pathname);
                closedir(dir);
        break;

    case PARM_R:
                if(lstat(pathname,&buf)==-1)
                    my_err("stat",__LINE__);
                if(S_ISDIR(buf.st_mode))
                {
                        count=0;
                        printf("\n\n%s:\n",pathname);
                        
                    if((dir=opendir(pathname))==NULL)
                        printf("权限不足\n");
                    while((ptr=readdir(dir))!=NULL)//用链表读取
                    {
                        if(ptr->d_name[0]!='.')
                            {
                                p=(LINK*)malloc(sizeof(LINK));
                                pwd=getcwd(NULL,0);
                                sprintf(p->name,"%s/%s%c",pathname,ptr->d_name,'\0');

                                count++;
                                p->next=NULL;
                                if(head==NULL)
                                    head=p;
                                else 
                                    q->next=p;
                                q=p;
                            }
                    }
                    closedir(dir);
                    p=head;
                    
                    for(i=1;i<=count;i++)//链表排序
                    {
                        p=head;
                        for(j=1;j<=count-i-1;j++)
                        {
                            if(strcmp(p->name,p->next->name)>0)
                            {
                                strcpy(t,p->name);
                                strcpy(p->name,p->next->name);
                                strcpy(p->next->name,t);
                            }
                            p = p->next;
                        }
                    }

                p=head;
                for(i=1;i<=count;i++)//先输出一部分内容
                    {
                        printf("%s ",p->name);
                        if(i%3==0)
                            putchar('\n');
                        p=p->next;
                    }
                    p=head;
                k=0;//记录目录个数
                    for(i=0;i<count;i++)//统计目录中目录个数
                        {
                            char *move;
                            if(stat(p->name,&buf)==-1)
                                my_err("stat",__LINE__);
                            
                            if(S_ISDIR(buf.st_mode))
                                 {
                                     //strcpy(alldir[k++],p->name);
                                  move=(char*)malloc(sizeof(int)*1000);
                                     strcpy(move,p->name);
                                     display(flag_param,move);
                                 }
                            free(move);

                            p=p->next;
                        }
                   /* if(k>0)//如果存在目录，就递归输出
                        {
                            for(i=0;i<k;i++) 
                            {
                                    display(flag_param,alldir[i]);
                            }
                            k=0;
                        }
                    }*/
               q=p=head;
                while(p!=NULL)
                {
                    q=p->next;
                    free(p);
                    p=q;
                }
                break;
        default:
            break;
        }
    
    }
}
/*void display_color(char *name)
{
    
}*/
