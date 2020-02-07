#include<stdio.h>
#include<unistd.h>
#include<errno.h>
#include<fcntl.h>
#include<sys/wait.h>
#include<string.h>
#define errorCode2 2
int command(const char* string, char *outbuf, int outlen, char * errbuf, int errlen);

int command(const char* string, 
                char *outbuf, int outlen, 
                char * errbuf, int errlen){
                    /* choosing the appropriate size for now since didn't know what to choose */
char* readBUFF[outlen];
char* writeBUFF[errlen];
char* readErrBUFF[outlen];
char* writeErrBUFF[errlen];
/* returning non zero incase of empty string */                    
int outputPipe, errorPipe, pid, nr;
int writeEndOutPipe,writeEndErrPipe;
if(string==NULL){
    return errorCode2;
}
if((outputPipe=pipe((int*)1))<0){
    fprintf(stderr,"Error:%s",strerror(errno));
}
if((errorPipe=pipe((int*)2))<0){
    fprintf(stderr,"Error:%s",strerror(errno));
}

if((pid=fork())<0){
    perror("fork error");
}else if(pid==0){
    /* child process */
    writeEndOutPipe=dup(1); /* for stdout */
    writeEndErrPipe=dup(2);   /* for stderr */
    if(execv(string,NULL)==-1){
        fprintf(stderr,"ERROR: %s",strerror(errno));
    }
    while((nr=read(outputPipe,readBUFF,outlen))>0){
        if(write(writeEndOutPipe,readErrBUFF,outlen)<0){
            fprintf(stderr,"ERROR: %s",strerror(errno));
        };
    };
    while((nr=read(outputPipe,readBUFF,errlen))>0){
        if(write(errorPipe,writeErrBUFF,errlen)<0){
            fprintf(stderr,"ERROR: %s",strerror(errno));
        };
    };

}else{
    wait((int*)2);
}
return 0;
}