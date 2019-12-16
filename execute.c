#include "sish.h"


/* 
    this function copies and creates a duplicate for the file descriptors  
*/
int redirect_to_std_input_output_from_fd(char* input,char* output,bool append){
    int fd;
    if (input) {
        if((fd = open(input, O_RDONLY)) == -1){
            perror("Invalid Input: \n");
            return EXIT_FAILURE;
        }
        if(dup2(fd, STDIN_FILENO) == -1){
            fprintf(stderr, "dup2 err,errno: %d\n",errno);
            return EXIT_FAILURE;
        }
        close(fd);
        }
    if (output) {
        if (append) {
            if((fd = open(output, O_CREAT | O_TRUNC | O_WRONLY, DFTPERMISSION)) == -1){
                perror("Cannot create file:\n");
                return EXIT_FAILURE;
            }
            if(dup2(fd, STDOUT_FILENO) == -1){
                perror("Dup2 error:\n");
                return EXIT_FAILURE;
            }
            close(fd);
        }
        if((fd = open(output, O_CREAT | O_APPEND | O_WRONLY, DFTPERMISSION)) == -1){
            perror("Cannot create file\n");
            return EXIT_FAILURE;
        }
        if(dup2(fd, STDOUT_FILENO) == -1){
            perror("Cannot create file \n");
            return EXIT_FAILURE;
        }
        close(fd);
    }
    return EXIT_SUCCESS;
}



/* 
    This function works for redirection and no_redirection as well. This is actually the core function
    which runs execvp and checks that if it's a redirection required then we will pass pass input,
    output to  std_in, std_out.

    If your background is true then you would fall in the endless realm by passing WNOHANG to waitpid 

*/
int execute_with_execvp(char* commands[],char* input,char* output,bool append,bool background_process,bool redirection){
    pid_t pid;
    int status;
    if ((pid = fork()) < 0) {
        perror("Fork Issue \n");
        return EXIT_FAILURE;
    }
    if (pid == 0) {
        if(signal(SIGINT, SIG_DFL)==SIG_ERR){
            perror("Signal Handler Issue in Fork \n");
        }
        if(redirection){
            if(redirect_to_std_input_output_from_fd(input,output,append)==EXIT_FAILURE){
                return EXIT_FAILURE;
            };
        }
        if (execvp(commands[0], commands) == -1) {
            perror("ERROR");
            kill(getpid(),SIGTERM);
        }
    }else { 
        if (!background_process) {
            waitpid(pid, &status, 0);
        }else {
            printf("Pid: %d\n",pid);
            waitpid(pid, NULL, WNOHANG);            
        }
    }
    return EXIT_SUCCESS;
}


/* executing if we have pipes in here */
int execute_pipe(char *commands[],int length_token){
    int fd[2];
	pid_t pid;
	int fdd = 0;	
    char command_in_string[MAX_LENGTH_COMMAND], *p;
    char *inner_breaking, *inner_last;
    char *last;
    int i = 0,j=0,k=0;
    char ***cmds;
    /* 
        converting the command into a string  
    */
    strcpy(command_in_string,commands[0]);
    strcat(command_in_string," ");
    for(int l=1;l<length_token;l++){
        strcat(command_in_string,commands[l]);
        strcat(command_in_string," ");
    }
    /* 
        creating a command through breaking and tokenizing it to create an array 
    */
    cmds = malloc(length_token*sizeof(char**));
    for ((p = strtok_r(command_in_string, "|", &last)); p;
        (p = strtok_r(NULL, "|", &last)), i++) {
            cmds[i] = malloc(length_token*sizeof(char*));
            j=0;
            for ((inner_breaking = strtok_r(p, " ", &inner_last)); inner_breaking;
                (inner_breaking = strtok_r(NULL, " ", &inner_last)), k++){
                    cmds[i][j]=inner_breaking;
                    j++;
                }
                cmds[i][j]=NULL;
    }
    /* main exection */
	while (*cmds != NULL) {
		if(pipe(fd)<0){
            perror("pipe error");
            return EXIT_FAILURE;
        };				
		if ((pid = fork()) == -1) {
			perror("fork");
			exit(1);
		}
		else if (pid == 0) {
			if(dup2(fdd, STDIN_FILENO)==-1){
                perror("Dup2 error:\n");
                return EXIT_FAILURE;
            };
			if (*(cmds + 1) != NULL) {
				if(dup2(fd[1], STDOUT_FILENO)==-1){
                   perror("Dup2 error:\n");
                   return EXIT_FAILURE; 
                };
			}
			close(fd[0]);
            if (execvp((*cmds)[0], *cmds) == -1) {
                perror("Not Valid Arguement:\n");
                kill(getpid(),SIGTERM);
            }
			exit(1);
		}
		else {
			wait(NULL); 		
			close(fd[1]);
			fdd = fd[0];
			cmds++;
		}
	}
    return EXIT_SUCCESS;
}