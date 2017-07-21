/*************************************************************************
	> File Name: test.c
	> Author: 
	> Mail: 
	> Created Time: 2017年07月21日 星期五 09时26分30秒
 ************************************************************************/

#include<stdio.h>
#include<string.h>
void swap(char *a, char *b)
{
        char t[256];
        strcpy(t,a);
        strcpy(a,b);
        strcpy(b,t);

}
int PART(char (*arr)[10],int p,int q)
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

void QUICK(char (*arr)[10],int p,int q)//快排!!!
{
        int r;
        if(p<q)
    {
                r=PART(arr,p,q);
                QUICK(arr,p,r-1);
                QUICK(arr,r+1,q);
            
    }

}
int main()
{
    char arr[5][10]={"dasjv", "cpoibab", "pppdavzc", "adfavz", "zcv"};
    QUICK(arr,0,3);
    for(int i=0;i<5;i++)
        printf("%s\n",arr[i]);
    return 0;
}
