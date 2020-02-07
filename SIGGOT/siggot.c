#include<stdio.h>
#include<unistd.h>
int main(int argc, char* argv[]){
    int pid;
    int shots=0;
    int defence=0;
    if((pid=fork())<0){
        fprintf(stderr,"Error while forking");
    }else if(pid==0){ /* if it's a child process */
        sleep(4); 
        shots++;
        kill()
    }else{
        sleep(4);
        shots++;
    }
}