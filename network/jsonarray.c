/*************************************************************************
	> File Name: jsonarray.c
	> Author: 
	> Mail: 
	> Created Time: 2017年08月13日 星期日 16时58分52秒
 ************************************************************************/

#include<stdio.h>
#include<cJSON.h>

#define JSONSTR "{\"type\":200, \"list\":[{\"username\":\"yuan\",\"type\":1}, {\"username\":\"haohao\",\"type\":0}]}"

int main() {
    cJSON *root=cJSON_Parse(JSONSTR);
    cJSON *list=cJSON_GetObjectItem(root,"list");
    
    int size=cJSON_GetArraySize(list);
    cJSON *p,*un,*typ;
    for (int i = 0; i < size; i++) {
        p=cJSON_GetArrayItem(list, i);     
        un=cJSON_GetObjectItem(p,"username"),
        typ=cJSON_GetObjectItem(p,"type");
        printf("username:%s,type:%d\n",un->valuestring,typ->valueint);
    }
    return 0;
}
