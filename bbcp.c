#include <stdio.h>
#include<stdlib.h>
#include <fcntl.h>
#include<unistd.h>
#include<errno.h>
#include<libgen.h>
#include<string.h>
#include<sys/stat.h>
#define EXIT_ERROR_CODE -1
#define BUFFERSIZE 16384

int main(int argc,char *argv[]){
    char buffer[BUFFERSIZE];
    struct stat buf,secondDirectory;
    int n,writeDescriptor,filedesc;
    /* if it's less than 2 parameters */
    if(argc<=2){
        fprintf(stderr,"Not sufficient parameters");
        exit(1);
    }
    stat(argv[1],&buf);
    stat(argv[2],&secondDirectory);

    if(S_ISDIR(buf.st_mode)){
        printf("Source is a directory\n");
        exit(1);
    }
    if(argc==3){
        char* source = argv[1];
        char* destination = argv[2];

        if(S_ISDIR(secondDirectory.st_mode)){
            strcat(destination,"/");
            strcat(destination,basename(source));
        }
        
        if(buf.st_ino==secondDirectory.st_ino){
            fprintf(stderr,"Symbolic link");
            exit(1);
        }
        if((filedesc = open(source,O_RDONLY)) ==-1){
            fprintf(stderr,"Unable to open %s",strerror(errno));
            exit(1);
        };
        if((writeDescriptor=open(destination,O_WRONLY|O_CREAT|O_TRUNC))==-1){
            fprintf(stderr,"Unable to write %s",strerror(errno));
            exit(1);
        };

        while((n = read(filedesc,buffer,BUFFERSIZE))>0){
            printf("Printing a line with buffer size 16384\n");
            if(write(writeDescriptor,buffer,n)!=n){
                    fprintf(stderr,"Error");
                    exit(1);
            };
        }
        close(writeDescriptor);
        close(filedesc);
    }
    if(argc>3){
        fprintf(stderr,"Too many arguements");
        exit(1);
    }
    exit(0);
}