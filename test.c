#include "sish.h"

int main(int argc,char* argv[]){
    
    char *commands[] = /* {"ls>file1","<file2",NULL}; */ {"ls >file1 < file2"};
    int j=0,k=0,i;
    char *parsed_commands[10];
        char *stri = commands[0];
        char *next = (stri+1);
        char *result; 
        // char *stri = "Geeks>for>Geeks"; 
        char* token;
        char* rest = strdup(stri); 
        int done=0;
        result = (char *)malloc(strlen(stri) + 3); 
        i=0;
        for ((p = strtok_r(commands, "|", &last)); p;
        (p = strtok_r(NULL, "|", &last)), i++) {
        }
        printf("%s\n",result);
    // printf("%s",parsed_commands[0]);
    return (0); 
}
