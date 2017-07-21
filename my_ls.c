/*************************************************************************
	> File Name: my_ls.c
	> Author: 
	> Mail: 
	> Created Time: 2017年07月20日 星期四 09时50分40秒
 ************************************************************************/

#include<stdio.h>
#include<stdio.h>
#include<string.h>
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


void swap(char *a, char *b);
int PART(char (*arr)[256],int p,int q);
void QUICK(char (*arr)[256],int p,int q);//快排!!!
void my_err(char *err_string,int line);
void display_attribute(struct stat buf,char *name);
void display(int flag_param,char *pathname);//传二维数组
void display_dir(int flag_param,char *name);

int main(int argc,char *argv[])
{
    int i,j,k=0;
    char param[10]={0};//存-a，-l
    int flag_param=PARM_NONE;
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
    if (flag==1)//如果没有’-‘，count=0
    {
        strcpy(name,"./");
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
        if(S_ISDIR(buf.st_mode))
            {
                //display_dir(flag_param,name);
                display(flag_param,name);
                i++;
            }
        else 
            {
                display(flag_param,name);
                i++;
            }
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

void display_attribute(struct stat buf,char *name)
{
    struct passwd *user;
    struct group *group;
    struct stat bu;
    if(stat(name,&bu)==-1)
        my_err("stat",__LINE__);
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
        
}

void display(int flag_param,char *pathname)//传二维数组
{
    DIR *dir;
    struct dirent *ptr;
    int count=0;
    struct stat buf;
    int i,j;
    j=0;
    if(stat(pathname,&buf)==-1)///
        my_err("lstat",__LINE__);

    switch(flag_param)
    {
        case PARM_NONE:
                if(pathname[0]!='.' && pathname[1] != '.')
            {
                if(S_ISDIR(buf.st_mode))
                    
                  {  count=0;
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
            }           
               else
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
                        printf("%s ",arr[i]);
                        k++;
                        if(k%5==0)
                            putchar('\n');
                    }
                    if(k%5!=0)
                        putchar('\n');
                    closedir(dir);
                }
        
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
                //printf("%s ",pathname);
                    closedir(dir);
            break;

        case PARM_L:
            if(pathname[0]!='.')//不输出隐藏文件
                display_attribute(buf,pathname);
            else 
                {
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
                            display_attribute(buf,arr[i]);
                        }
                    }
                    else
                        display_attribute(buf,pathname);
                    closedir(dir);
                }
        break;

        case PARM_L+PARM_A:
               count=0;
                if((dir=opendir(pathname))==NULL)
                    my_err("opendir",__LINE__);
                while((ptr=readdir(dir))!=NULL)////要排序
                        strcpy(arr[count++],ptr->d_name);

                QUICK(arr,0,count-1);

                    for(i=0;i<count;i++)
                        display_attribute(buf,arr[i]);
                   closedir(dir);
        break;
        default:
            break;
    }
}

//读取目录文件
/*void display_dir(int flag_param,char *name)
{
    DIR *dir;//要打开的目录
    struct dirent *ptr;//结构体蕴含文件长度，名字
    int i=0,j;//计数
   // struct stat buf;
    int len;
    if((dir=opendir(name))==NULL)
        my_err("opendir",__LINE__);

    //统计最长文件名
   while((ptr=readdir(dir))!=NULL) 
    {
        if(strlen(ptr->d_name)>maxfilename)
            maxfilename=strlen(ptr->d_name);
    }
    closedir(dir);

    if((dir=opendir(name))==NULL)
        my_err("opendir",__LINE__);
    //开始把文件名存放在数组中
    while((ptr=readdir(dir))!=NULL)
    {
        strcpy(arr[i],ptr->d_name);
        len=strlen(ptr->d_name);
        arr[i][len]='\0';
        i++;
    }

    //对数组进行排序,按字母顺序排序
    QUICK(arr,1,i-1);
    for(j=0;j<i;j++)
       display(flag_param,arr[j]);
    closedir(dir);
}*/

