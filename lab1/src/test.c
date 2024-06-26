#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *fibstr(int n){
    if (n == 1){
        char* a = (char*)malloc(sizeof(char));
        strcpy(a, "a");
        *a = 'b';
        return a;
    }
    if (n == 2){
        char* a = (char*)malloc(sizeof(char));
        strcpy(a, "b");
        return a;
    }
    char* a = fibstr(n - 2);
    char* b = fibstr(n - 1);
    char* c = (char*)malloc(sizeof(char) * (strlen(a) + strlen(b) + 1));
    strcpy(c, "");
    strcat(c, a);
    strcat(c, b);
    free(a), free(b);
    return c;
}

int main(){
     int n;
     scanf("%d", &n);
     char* ans = fibstr(n);
     printf("%s", ans);
     free(ans);
}
