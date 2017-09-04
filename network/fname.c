#include<stdlib.h>
#include<stdio.h>
#include<string.h>

void get_filename(char *name) {
    char *buf = (char *)malloc(strlen(name));
    strcpy(buf, name);
    char *pt = strrchr(buf, '/');
    if (pt != NULL) {
        strcpy(name, pt + 1);
    }
    free(buf);
}