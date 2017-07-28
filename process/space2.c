#include<stdio.h>
#include<string.h>
int find_length(const char *src, int from)
{
    int ret=0, len=strlen(src);
    while(src[from+ret]!=' '&&from+ret<len)
        ret++;
    return ret;
}

int find_nextnoblank(const char *src, int from)
{
    int ret=0, len=strlen(src);
    while(src[from+ret]==' '&&from+ret<len)
        ret++;
    return from+ret;
}

int main()
{
    char str[1000];
    char buf[100][256];
    int k=0;

    gets(str);
    int start=0,end=0,lg;

    while(end<strlen(str))
    {
        start=find_nextnoblank(str,end);
        lg=find_length(str,start);
        end=start+lg+1;
        strncpy(buf[k++],str+start,lg);
        if (strcmp(buf[k-1],"")==0)
            k--;
    }


    for(int z=0;z<k;z++)
    {
        printf("#%s#\n", buf[z]);
    }
}
