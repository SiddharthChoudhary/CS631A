#include<stdio.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<limits.h>
#include <unistd.h>

int main(int argc, char* argv[]){
char c[PATH_MAX];
char* destination=argv[1];
struct stat buf;
stat(argv[1],&buf);
if(!S_ISDIR(buf.st_mode)){
        printf("Not a directory\n");
        exit(1);
}
chdir(destination);
printf("The current directory Now is %s \n",getcwd(c, PATH_MAX));
return 0;
}

