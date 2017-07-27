#include<stdio.h>
#include<string.h>

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
        q=0;
        if(buf[i]==' ')
        {
            start=i-1;
            for(j=end;j<=start;j++)
            {
                flag=1;
                str[k][q++]=buf[j];
            }
            end=i+1;
        }
        if(flag)
        {
            str[k][q]='\0';
            k++;
        }
    }
    return k;
}

int main() {
    char st[256][256];
    char buf[1000];
    gets(buf);
    printf("%s\n", buf);
    get_param(buf, st);

    for (int i = 0; i < 10; i++) {
        printf("%s#\n", st[i]);
    }

    return 0;
}
