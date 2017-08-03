#include<stdio.h>
#include<dlfcn.h>
#include<stdlib.h>
int main()
{
    void *handle;
    void (*welcome)();
    char *error;

    if((handle=dlopen("./libtt.so",RTLD_LAZY))==NULL)//打开动态库
    {
        printf("dlopen error\n");
        exit(1);
    }
    welcome=dlsym(handle,"welcome");//获得welcome函数地址
    if((error=dlerror())!=NULL)
    {
        printf("dlsym error\n");
        exit(1);
    }
    welcome();
    dlclose(handle);//关闭动态库

    exit(0);
}
