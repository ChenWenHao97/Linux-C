/*************************************************************************
	> File Name: jeson.c
	> Author: 
	> Mail: 
	> Created Time: 2017年08月09日 星期三 14时51分37秒
 ************************************************************************/

#include<stdio.h>
#include"cJSON.h"
#include<string.h>
#include<stdlib.h>
/*
{
    "from":"cwh",
    "to":"lzh",
    "context":"55"
}
*/
int main()
{
    cJSON *root,*fmt;
    root=cJSON_CreateObject();
    cJSON_AddStringToObject(root,"from","cwh");
    cJSON_AddStringToObject(root,"to","lzh");
    cJSON_AddStringToObject(root,"context","55");
    char *out=cJSON_Print(root);
    printf("%s\n",out);
    cJSON_Delete(root);
    free(out);
    //当server收到了out之后
    //cJSON *rv = cJSON_Parse(out);
    //cJSON *from=cJSON_GetObjectItem(rv,"from");
    //printf("from=%s",from->valuestring);
}
