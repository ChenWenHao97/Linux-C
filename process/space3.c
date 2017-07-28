#include<stdio.h>
#include<string.h>
int getlenth(int start,char *buf);
int getstart(int end,char *buf);
int main()
{
    char buf[2000];
    char str[100][256];
    int start=0,end=0,k=0;
    int flag;
    gets(buf);
    while(end<strlen(buf))
    {
        start=getstart(start,buf);
        int lenth=getlenth(start,buf);
        end=start+lenth+1;
        printf("end is %d\n",end);
        strncpy(str[k++],buf+start,lenth+1);
        if(strcmp(str[k-1],"\0")==0)
            k--;
        printf("k is =%d\n",k);
        start=end;

    }
    for(int i=0;i<k;i++)
        printf("###%s###\n",str[i]);
}
int getlenth(int start,char *buf)
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
    sleep(5);
    return lenth;
}
int getstart(int start,char * buf)
{
    while(buf[start]==' ' && start<=strlen(buf))
        start++;
    printf("start=%d\n",start);
    sleep(5);
    return start;

}
