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
#define BLACK  "\033[30m"
#define ORANGE  "\033[31m"
#define GREEN  "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE  "\033[34m"
#define PINK  "\033[35m"
#define WHITE  "\033[37m"
#define SKY "\033[36m"
#define CANCEL "\033[0m"
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
void color(char *name,char *pathname);

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

    char str[256];
    sprintf(str,"%s/%s",pathname,name);//构建绝对路径
    if(lstat(str,&bu)==-1)
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

    printf("%-4d",bu.st_nlink);//链接数
    printf("%-5s",user->pw_name);//user;
    printf("%-5s",group->gr_name);//group;
    printf("%-8d",bu.st_size);//文件大小
    char time_buf[255];
    strcpy(time_buf, ctime(&bu.st_atime));
    time_buf[strlen(time_buf)-1] = '\0';//ctime会有一个换行
    printf("%  s  ",time_buf);//文件时间
    if(name[0]!='\0')
        {
            color(name,pathname);
            putchar('\n');
        }

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
    char *pwd;
    int k;

    if(lstat(pathname,&buf)==-1)
    {
        my_err("lstat",__LINE__);
    }

    switch(flag_param)
    {
        case PARM_NONE:
        {
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
                            color(arr[i],pathname);
                            printf("  ");
                            k++;
                            if(k%5==0)
                                putchar('\n');
                        }
                        if(k%5!=0)
                            putchar('\n');
                        closedir(dir);
                  }
                else
                     //color(pathname);
                    printf(SKY "%s" CANCEL,pathname);//文件
        }
        break;

        case PARM_A:
        {
                count=0;
                if((dir=opendir(pathname))==NULL)
                    my_err("opendir",__LINE__);
                while((ptr=readdir(dir))!=NULL)////要排序
                        strcpy(arr[count++],ptr->d_name);

                    QUICK(arr,0,count-1);
                    int k=0;
                    for(i=0;i<count;i++)
                    {
                        color(arr[i],pathname);//arr[i]);
                        printf("  ");
                        k++;
                        if(k%5==0)
                            putchar('\n');
                    }
                    if(k%5!=0)
                        putchar('\n');
            closedir(dir);
        }
            break;

        case PARM_L:
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
                                display_attribute(buf,arr[i],pathname);
                        }
                         closedir(dir);
                    }
                    else
                        display_attribute(buf,".",pathname);
            }
        break;

    case PARM_L+PARM_A:
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
                        display_attribute(buf,arr[i],pathname);
               }
               else
                    display_attribute(buf,".",pathname);
                closedir(dir);
	}
        break;
    case PARM_A+PARM_R:
	{
            LINK *p1,*q1,*head1;
            LINK *p2,*q2,*head2;
		    if(S_ISDIR(buf.st_mode))
            {
                q1 = head1 = (LINK*)malloc(sizeof(LINK));
                strcpy(head1->name, pathname);
                q1->next=NULL;
                while (head1 != NULL)
                {
                    char thisname[256];
                    strcpy(thisname, head1->name);
                    printf(BLUE "%s"CANCEL,thisname);//目录
                   // color(thisname);
                    printf(":\n");
                    head2=NULL;
                    if ((dir = opendir(thisname)) == NULL)
                    {
                        perror("opendir");
                        p1 = head1;
                        head1 = head1->next;
                        free(p1);
                        continue;
                    }
                    count=0;
                    while((ptr=readdir(dir))!=NULL)//创建文件链表
                    {
                        p2=(LINK*)malloc(sizeof(LINK));
                        strcpy(p2->name,ptr->d_name);
                        count++;
                        p2->next=NULL;
                        if(head2==NULL)
                            head2=p2;
                        else
                            q2->next=p2;
                        q2=p2;
                    }
                    closedir(dir);
                    for(i=1;i<=count;i++)
                    {
                        p2=head2;
                        for(j=1;j<=count-i;j++)
                        {
                            if(strcmp(p2->name,p2->next->name)>0)
                                swap(p2->name,p2->next->name);
                            p2=p2->next;
                        }
                    }
                    int k=0;
                    for (p2=head2; p2!=NULL; p2=p2->next)
                    {
                        char full_dir[256];
                        sprintf(full_dir,"%s/%s",thisname,p2->name);
                        if(lstat(full_dir,&buf)==-1)
                        {
                            perror("\nlstat");
                        }
                        else
                        {
                            if(S_ISDIR(buf.st_mode))
                            {
                                p1=(LINK*)malloc(sizeof(LINK));
                                strcpy(p1->name,full_dir);
                                p1->next=NULL;
                                q1->next=p1;
                                q1=p1;
                            }
                        }

                        color(p2->name,thisname);
                        printf("  ");
                        k++;
                        if(k%5==0)
                            putchar('\n');
                    }
                    putchar('\n');
                    p2=head2;
                    while(p2!=NULL)
                    {
                        q2=p2->next;
                        free(p2);
                        p2=q2;
                    }
                    p1 = head1;
                    head1 = head1->next;
                    free(p1);

                }
            }
            else
                {
                    //color(pathname);
                    printf(SKY "%s" CANCEL,pathname);//文件
                    putchar('\n');
	            }
    }
break;
    case PARM_L+PARM_R:
    {
	        LINK *p1,*q1,*head1;
            LINK *p2,*q2,*head2;
            if(S_ISDIR(buf.st_mode))
            {
                q1 = head1 = (LINK*)malloc(sizeof(LINK));
                strcpy(head1->name, pathname);
                q1->next=NULL;
                while (head1 != NULL)
                {
                    char thisname[256];
                    strcpy(thisname, head1->name);
                    //color(thisname);
                     printf(BLUE "%s"CANCEL,thisname);//目录
                    printf(":\n");
                    head2=NULL;
                    if ((dir = opendir(thisname)) == NULL)
                    {
                        perror("opendir");
                        p1 = head1;
                        head1 = head1->next;
                        free(p1);
                        continue;
                    }
                    count=0;
                    while((ptr=readdir(dir))!=NULL)//创建文件链表
                    {
                        p2=(LINK*)malloc(sizeof(LINK));
                        if(ptr->d_name[0]=='.')
                            continue;
                        strcpy(p2->name,ptr->d_name);
                        count++;
                        p2->next=NULL;
                        if(head2==NULL)
                            head2=p2;
                        else
                            q2->next=p2;
                        q2=p2;
                    }
                    closedir(dir);
                    for(i=1;i<=count;i++)
                    {
                        p2=head2;
                        for(j=1;j<=count-i;j++)
                        {
                            if(strcmp(p2->name,p2->next->name)>0)
                                swap(p2->name,p2->next->name);
                            p2=p2->next;
                        }
                    }
                    int k=0;
                    for (p2=head2; p2!=NULL; p2=p2->next)
                    {
                        char full_dir[256];
                        sprintf(full_dir,"%s/%s",thisname,p2->name);
                        if(lstat(full_dir,&buf)==-1)
                        {
                            perror("\nlstat");
                        }
                        else
                        {
                            if(S_ISDIR(buf.st_mode))
                            {
                                p1=(LINK*)malloc(sizeof(LINK));
                                strcpy(p1->name,full_dir);
                                p1->next=NULL;
                                q1->next=p1;
                                q1=p1;
                            }
                        }

			        display_attribute(buf,p2->name,thisname);
                    }
                    putchar('\n');
                    p2=head2;
                    while(p2!=NULL)
                    {
                        q2=p2->next;
                        free(p2);
                        p2=q2;
                    }
                    p1 = head1;
                    head1 = head1->next;
                    free(p1);

                }
            }
            else
            {
                printf(BLUE "%s"CANCEL,pathname);//目录
                printf("\n");
            }
    }
	break;

    case PARM_L+PARM_R+PARM_A:
	{
		LINK *p1,*q1,*head1;
        LINK *p2,*q2,*head2;
            if(S_ISDIR(buf.st_mode))
            {
                q1 = head1 = (LINK*)malloc(sizeof(LINK));
                strcpy(head1->name, pathname);
                q1->next=NULL;
                while (head1 != NULL)
                {
                    char thisname[256];
                    strcpy(thisname, head1->name);
                    printf(BLUE "%s"CANCEL,thisname);//目录
                    printf(":\n");
                    head2=NULL;
                    if ((dir = opendir(thisname)) == NULL)
                    {
                        perror("opendir");
                        p1 = head1;
                        head1 = head1->next;
                        free(p1);
                        continue;
                    }
                    count=0;
                    while((ptr=readdir(dir))!=NULL)//创建文件链表
                    {
                        p2=(LINK*)malloc(sizeof(LINK));
                        strcpy(p2->name,ptr->d_name);
                        count++;
                        p2->next=NULL;
                        if(head2==NULL)
                            head2=p2;
                        else
                            q2->next=p2;
                        q2=p2;
                    }
                    closedir(dir);
                    for(i=1;i<=count;i++)
                    {
                        p2=head2;
                        for(j=1;j<=count-i;j++)
                        {
                            if(strcmp(p2->name,p2->next->name)>0)
                                swap(p2->name,p2->next->name);
                            p2=p2->next;
                        }
                    }
                    int k=0;
                    for (p2=head2; p2!=NULL; p2=p2->next)//存放目录文件
                    {
                        char full_dir[256];
                        sprintf(full_dir,"%s/%s",thisname,p2->name);
                        if(lstat(full_dir,&buf)==-1)
                        {
                            perror("\nlstat");
                        }
                        else
                        {
                            if(S_ISDIR(buf.st_mode)&&strcmp(p2->name,".")!=0&&strcmp(p2->name,"..")!=0)
                            {
                                p1=(LINK*)malloc(sizeof(LINK));
                                strcpy(p1->name,full_dir);
                                p1->next=NULL;
                                q1->next=p1;
                                q1=p1;
                            }
                        }

			        display_attribute(buf,p2->name,thisname);
                    }
                    putchar('\n');
                    p2=head2;
                    while(p2!=NULL)
                    {
                        q2=p2->next;
                        free(p2);
                        p2=q2;
                    }
                    p1 = head1;
                    head1 = head1->next;
                    free(p1);

                }
            }
            else
            {
                printf(BLUE "%s"CANCEL,pathname);//目录
                printf("\n");
            }
	}
	break;

    case PARM_R:
    {
            LINK *p1,*q1,*head1;
            LINK *p2,*q2,*head2;
            if(S_ISDIR(buf.st_mode))
            {
                q1 = head1 = (LINK*)malloc(sizeof(LINK));
                strcpy(head1->name, pathname);
                q1->next=NULL;
                while (head1 != NULL)
                {
                    char thisname[256];
                    strcpy(thisname, head1->name);
                     printf(BLUE "%s"CANCEL,thisname);//目录
                    printf(":\n");
                    head2=NULL;
                    if ((dir = opendir(thisname)) == NULL)
                    {
                        perror("opendir");
                        p1 = head1;
                        head1 = head1->next;
                        free(p1);
                        continue;
                    }
                    count=0;
                    while((ptr=readdir(dir))!=NULL)//创建文件链表
                    {
                        p2=(LINK*)malloc(sizeof(LINK));
                        if(ptr->d_name[0]=='.')
                            continue;
                        strcpy(p2->name,ptr->d_name);
                        count++;
                        p2->next=NULL;
                        if(head2==NULL)
                            head2=p2;
                        else
                            q2->next=p2;
                        q2=p2;
                    }
                    closedir(dir);
                    for(i=1;i<=count;i++)
                    {
                        p2=head2;
                        for(j=1;j<=count-i;j++)
                        {
                            if(strcmp(p2->name,p2->next->name)>0)
                                swap(p2->name,p2->next->name);
                            p2=p2->next;
                        }
                    }
                    int k=0;
                    for (p2=head2; p2!=NULL; p2=p2->next)
                    {
                        char full_dir[256];
                        sprintf(full_dir,"%s/%s",thisname,p2->name);
                        if(lstat(full_dir,&buf)==-1)
                        {
                            perror("\nlstat");
                        }
                        else
                        {
                            if(S_ISDIR(buf.st_mode))
                            {
                                p1=(LINK*)malloc(sizeof(LINK));
                                strcpy(p1->name,full_dir);
                                p1->next=NULL;
                                q1->next=p1;
                                q1=p1;
                            }
                        }
                        color(p2->name,thisname);

                        printf("  ");
                        k++;
                        if(k%5==0)
                            putchar('\n');
                    }
                    putchar('\n');
                    p2=head2;
                    while(p2!=NULL)
                    {
                        q2=p2->next;
                        free(p2);
                        p2=q2;
                    }
                    p1 = head1;
                    head1 = head1->next;
                    free(p1);
                }
            }
            else
            {
                 printf(BLUE "%s"CANCEL,pathname);//目录
                printf("\n");
            }

    }
 	break;
    default:
        break;
    }
}

void color(char *name,char *pathname )//颜色函数
{
    struct stat bu;
    char buf[256];
    int flag=0;
    if(pathname[strlen(pathname)-2]!='/'&&name[0]!='/')
        sprintf(buf,"%s/%s",pathname,name);
    else if(pathname[strlen(pathname)-2]!='/'&&name[0]=='/')
        sprintf(buf,"%s%s",pathname,name);
    else
        sprintf(buf,"%s%s",pathname,name);
    if(lstat(buf,&bu)==-1)
    {
        flag=1;
        //printf(",权限不足！\n");
    }
    if(S_ISLNK(bu.st_mode))//判断文件是什么类型
     printf(BLUE "%s"CANCEL,name);//目录
    else if(S_ISDIR(bu.st_mode))
        printf(ORANGE "%s" CANCEL,name);//链接
    else if(S_ISCHR(bu.st_mode))
    printf(GREEN "%s"CANCEL,name);//字符
    else if(S_ISBLK(bu.st_mode))
      printf(PINK "%s"CANCEL,name);//块
    else if(S_ISFIFO(bu.st_mode))
    printf(WHITE "%s"CANCEL,name);//FIFO
    else if(S_ISSOCK(bu.st_mode))
     printf(BLACK"%s"CANCEL,name);//socket
    else
        printf(SKY "%s" CANCEL,name);//文件
    if(flag)
        printf("权限不足！\n");

}
