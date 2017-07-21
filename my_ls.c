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
    char param[10];//存-a，-l
    int flag_param=PARM_NONE;
    struct stat buf;
    char name[20];
    int count=0;//计数-

    for(i=1;i<argc;i++)
    {
        if(argv[i][0]=='-')//证明有参数
        {

            for(j=1;j<strlen(argv[i]);j++)
                param[k++]=argv[i][j];
            count++;
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
    }
//判断后面有没有参数
    if((count+1)==argc)//如果没有’-‘，count=0
    {
        strcpy(name,"./");
        display(flag_param,name);
    }
    i=1;
    while(i < argc)
    {
        if(argv[i][0]=='-')
        {
            i++;
            continue;
        }
        else 
            strcpy(name,argv[i]);
        if((stat(name,&buf))==-1)
            my_err("stat",__LINE__);
        if(S_ISDIR(buf.st_mode))
            {
                display_dir(flag_param,name);
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
        if(arr[i][0]<arr[q][0])
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
    if(S_ISLNK(buf.st_mode))//判断文件是什么类型
        putchar('l');
    else if(S_ISDIR(buf.st_mode))
        putchar('d');
    else if(S_ISCHR(buf.st_mode))
        putchar('c');
    else if(S_ISBLK(buf.st_mode))
        putchar('b');
    else if(S_ISFIFO(buf.st_mode))
        putchar('f');
    else if(S_ISSOCK(buf.st_mode))
        putchar('s');
    else
        putchar('-');

    //判断user权限
    if(buf.st_mode & S_IREAD)
        putchar('r');
    else if(buf.st_mode & S_IWRITE)
        putchar('w');
    else if(buf.st_mode & S_IEXEC)
        putchar('x');
    else 
        putchar('-');

    //判断group权限
    if(buf.st_mode & S_IRGRP)
        putchar('r');
    else if(buf.st_mode & S_IWGRP)
        putchar('w');
    else if(buf.st_mode & S_IXGRP)
        putchar('x');
    else 
        putchar('-');

    //判断others权限
    if(buf.st_mode & S_IROTH)
        putchar('r');
    else if(buf.st_mode & S_IWOTH)
        putchar('w');
    else if(buf.st_mode & S_IXOTH)
        putchar('x');
    else
        putchar('-');

    user=getpwuid(buf.st_uid);//获取userID
    group=getgrgid(buf.st_gid);//获取groupID

    printf("-%2d",buf.st_nlink);//链接数
    printf("-%8d",user->pw_name);//user;
    printf("-%8d",group->gr_name);//group;
    printf("-%8d",buf.st_size);//文件大小
    printf("-%10d",buf.st_atime);//文件时间

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
    char name[50];//maxfilename];
    j=0;
    /*int sz = strlen(pathnames);
    for(i=0;i<sz;i++)
    {
        if(pathnames[i]!='/')
            name[j++] = pathnames[i];
    }
    name[j]='\0';*/
    if(lstat(pathname,&buf)==-1)
        my_err("lstat",__LINE__);

    switch(flag_param)
    {
        case PARM_NONE:
               /* if(S_ISDIR(buf.st_mode))//是目录的情况
            {
                if((dir=opendir(pathname))==NULL)
                    my_err("opendir",__LINE__);
                else
                {
                    while((ptr=readdir(dir))!=NULL)
                        printf("%s",ptr->d_name);
                }
                closedir(dir);
                putchar('\n');
            }
                else*/
                if(pathname[0]!='.' && pathname[1] != '.')
                    printf("%d\n",pathname);
                else
                {
                    count=0;
                    if((dir=opendir(pathname))==NULL)
                        my_err("opendir",__LINE__);
                    while((ptr=readdir(dir))!=NULL)////要排序
                    {
                        if(ptr->d_name[0]!='.')
                        {
                            strcpy(arr[count++],ptr->d_name);
                        }
                           // printf("%s  ",ptr->d_name);
                    }
                    QUICK(arr,1,i-1);
                    for(i=0;i<count;i++)
                        printf("%s ",arr[i]);
                    putchar('\n');
                    closedir(dir);
                }
        
        break;
        
        case PARM_A: 
                if((dir=opendir(name))==NULL)
                    my_err("opendir",__LINE__);
                else
                {
                    while((ptr=readdir(dir))!=NULL)
                        printf("%s",ptr->d_name);
                }
                closedir(dir);
                putchar('\n');
            break;

        case PARM_L:
            if(pathname[0]!='.')//不输出隐藏文件
                display_attribute(buf,name);
        break;

        case PARM_L+PARM_A:
            display_attribute(buf,name);
        break;
        default:
            break;
    }
}

//读取目录文件
void display_dir(int flag_param,char *name)
{
    DIR *dir;//要打开的目录
    struct dirent *ptr;//结构体蕴含文件长度，名字
    int i=0,j;//计数
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
    
    //i=i-1;
    printf("%d\n", i);
    //对数组进行排序,按字母顺序排序
    //QUICK(arr,1,i-1);
    for(j=0;j<i;j++){
        
    puts(arr[j]);
        display(flag_param,arr[j]);

    }

}


