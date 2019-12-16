#include "sish.h"
int x_flag=0;
int c_flag=0;
int main(int argc, char** argv){
    char* command;
    /* Signal Handling first */
    if(signal(SIGINT, SIG_IGN)==SIG_ERR){
        perror("Couldn't handle SIGINT");
    };
    setEnv_SHELL();
    command=setFlags(argc,argv);

    if(c_flag){
        flow_if_c_flag_is_on(command);
    }else{
        flow_if_otherwise();
    }
}

void setEnv_SHELL(){
    char buf[BUF_SIZE];
    if(getcwd(buf, BUFSIZ) == NULL)
        perror("Cannot get current working directory");
    if(setenv("SHELL", buf, 1) == -1)
        perror("Cannot set environment variable");
}

char* setFlags(int argc,char** argv){
    char* command;
    char opt;
    while((opt = getopt(argc, argv, "xc:")) != -1){
        switch(opt){
        case 'x' :
            x_flag = 1;
            break;
        case 'c' :
            c_flag = 1;
            command = optarg;
            break;
        case '?':
            fprintf(stderr,SISH_ERROR);
            exit(EXIT_FAILURE);
        }
    }
    return command;
}