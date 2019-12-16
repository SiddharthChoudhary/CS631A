#include "sish.h"
void flow_if_c_flag_is_on(char* command){
    char *commands[MAXTOKENS];
    int num=0;
    if(command != NULL){
        if ((commands[0] = strtok(command, " \t\n")) == NULL){
            fprintf(stderr,SISH_ERROR);
            exit(EXIT_FAILURE);
        }
        num++;
        for(;(commands[num] = strtok(NULL, " \t\n")) != NULL;num++);
        execute_commands(num,commands);
    }

}

void flow_if_otherwise(){
    int length_of_input=0,num;
    char *command[MAXTOKENS];
    char* input;
    int length_of_array;
    size_t length_accepted;
    /* Since we need to let it run forever and keep waiting for the user's input. Once we get the input we will check for initial checks and then process it further */
    while (true) {
            num=0;
            printf("sish$ ");
            input = NULL;
            if ((length_of_input = getline(&input, &length_accepted, stdin)) == -1){
                fprintf(stderr, "getline err\n");
                continue;
            }
            input[length_of_input - 1] = '\0';
            if(strlen(input) == 0)
                continue;
            input = parse_input_string(input);
            if (strcmp(input, "exit") == 0){
                if (x_flag)
                    fprintf(stderr,"+ exit\n");
                exit(EXIT_SUCCESS);
            }
            if (input == NULL) {
                fprintf(stderr, "read error");
                continue;
            }
            if ((command[0] = strtok(input, " \t")) == NULL) {
                fprintf(stderr,"sish: No commands\n");
                continue;
            }
            num++;
            length_of_array = sizeof(command)/sizeof(command[0]) ;
            command[length_of_array-1] = '\0';

            for(;(command[num] = strtok(NULL, " \t") ) != NULL;num++);

            if(num>0){
                execute_commands(num,command);
            }else{
                fprintf(stdout,"EMPTY COMMAND\n");
            }
        }
}

/* 
*  There are four kind of execution in here happening;
*    1. if it contains builtin commands, then follow builtin command execution
*    2. if it follows pipes, then pipeline execution
*    3. if it contains redirect IOs, then redirect execution
*    4. else simple execution 
*/

int execute_commands(int num,char *commands[]){
    int i=0, num_of_pipes=0,m=0;
    bool if_pipe=false;
    bool if_redirection_io_with_pipe=false;
    char **duplicate;

    duplicate = malloc(num+1);
    /* because memset and bzero didn't work for me */
    while(duplicate[m]!=NULL){
        duplicate[m]=0;
        m++;
    }   
    bool background = false;
    if(strcmp("&",commands[num-1])==0){
        background = true;
        commands[num-1]="\0";
        num=num-1;
    }
    /* since builtin commands have higher precedence that's why we need to process them first */
    if(strcmp("cd",commands[0])==0 || strcmp("exit",commands[0])==0 || strcmp("echo",commands[0])==0){
        return call_builtin_cd_exit_echo(num,commands);
    }

/*  check if the command has pipe in it, if yes then pass the whole command in 
    execute_pipe processing (a different way of processing the data) */
    for(;commands[i]!=NULL;i++){
        if(strcmp("|",commands[i])==0){
            if_pipe=true;
            num_of_pipes++;
            if(strcmp(">",commands[i])==0 || strcmp("<",commands[i])==0 || strcmp(">>",commands[i])==0){
                if_redirection_io_with_pipe=true;
            }
        }
    }
    if(if_redirection_io_with_pipe){
        printf("We are not handling Redirects IO with Pipes\n");
    }
    if(if_pipe){
        execute_pipe(commands,num);
    }else{
        if (x_flag) {
            fprintf(stderr, "+ ");
            for(int lm=0;commands[lm]!=NULL;lm++){
                fprintf(stderr,"%s ",commands[lm]);
            }
            fprintf(stderr,"\n");
        }
        if(check_if_redirect_io_and_create_a_copy(commands,duplicate)){
            /* call the redirect input execution */
            duplicate[num]=NULL;
            if(redirect_commands(commands,duplicate,num,background)==EXIT_FAILURE){
                return EXIT_FAILURE;
            };
        }else{
            /* else just plain forking and execution by passing input, output as NULL,NULL 
                and append as false, and redirection as false
            */
            duplicate[num]=NULL;
            if(execute_with_execvp(duplicate,NULL,NULL,false,background,false)==EXIT_FAILURE){
                return EXIT_FAILURE;
            };
        }
    }
    return EXIT_SUCCESS;
}
/* calling builtin in case of cd,exit, and echo */
int call_builtin_cd_exit_echo(int number_of_commands,char *commands[]){
    if(strcmp("exit",commands[0])==0){
        /* because we need to display + in case of x_flag is on, otherwise execute exit directly */
        if(x_flag){
            fprintf(stdout,X_FLAG_OUTPUT);
            for(int i=0;i<number_of_commands;i++){
                fprintf(stdout,"%s ",commands[i]);
            }
            fprintf(stdout,"\n");
        }
        exit(EXIT_SUCCESS);
    }else if(strcmp("cd",commands[0])==0){
        /* in case, there is no directory mentioned. Thus, chdir to home directory */
        if(number_of_commands<1 || commands[1]==NULL){
            if(chdir(getenv("HOME"))==-1){
                fprintf(stderr,"HOME is not set: %s\n",strerror(errno));
                return EXIT_FAILURE;
            }
            if(x_flag){
                fprintf(stdout,"+cd \n");
            }
            return EXIT_SUCCESS;
        }else{
            if(x_flag){
                fprintf(stdout,X_FLAG_OUTPUT);
                for(int i=0;i<number_of_commands;i++){
                    fprintf(stdout,"%s ",commands[i]);
                }
                fprintf(stdout,"\n");
            }
            if (chdir(commands[1]) < 0) {
                fprintf(stderr,"cd: %s: %s\n",commands[1],strerror(errno));
                return EXIT_FAILURE;
            }
            return EXIT_SUCCESS;
        }
    }else if(strcmp("echo",commands[0])==0){
        int i = 0,file_descriptor,output,j=1;
        // duplicating the std_out in case of redirection
        if((output = dup(STDOUT_FILENO)) == -1){
            perror("Cannot duplicate");
            return  EXIT_FAILURE;
        }
        if(x_flag){
            fprintf(stdout,X_FLAG_OUTPUT);
            for(int k=0;k<number_of_commands;k++){
                fprintf(stdout,"%s ",commands[k]);
            }
            fprintf(stdout,"\n");
        }
        while (commands[i] != NULL) {
            char *filename = malloc( MAXTOKENS * sizeof(char));
            if (strcmp(">", commands[i]) == 0 || strcmp(">>", commands[i]) == 0){
                sprintf(filename,"%s",commands[i+1]);
                if (filename == NULL) {
                    perror("wrong format:No filename specified\n");
                    return EXIT_FAILURE;
                }
                /* there's only one difference in >> and >, i.e., the permissions which are being sent in open */
                if (strcmp(">", commands[i]) == 0){
                    if((file_descriptor = open(filename, O_WRONLY|O_CREAT)) == -1){
                        fprintf(stderr,"sish: %s: Error\n",filename);
                        return EXIT_FAILURE;
                    }
                }else if(strcmp(">>", commands[i]) == 0){
                    if((file_descriptor = open(commands[i + 1], O_CREAT | O_APPEND | O_WRONLY, DFTPERMISSION)) == -1){
                        fprintf(stderr,"sish: %s: Error \n",filename);
                        return EXIT_FAILURE;
                    }
                }
                if(dup2(file_descriptor, STDOUT_FILENO) == -1){
                    perror("Error:\n");                    
                    return  EXIT_FAILURE;
                }
                close(file_descriptor);
                break;
            }
            i++;
        }
        while(j<i){
            if(strcmp("$$", commands[j]) == 0)
                printf("%d ",(int)getpid());
            else
                printf("%s ",commands[j]);
            j++;
        }
        printf("\n");
        if(dup2(output, STDOUT_FILENO) == -1){
            perror("Error");
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}

bool check_if_redirect_io_and_create_a_copy(char *commands[],char *duplicate[]){
    bool if_redirect;
     for(int i=0;commands[i]!=NULL;i++) {
        if((strcmp(">", commands[i]) == 0) || (strcmp(">>", commands[i]) == 0) || (strcmp("<", commands[i]) == 0)){
                if_redirect=true;
                break;
             }
        duplicate[i] = commands[i];
    }
    return if_redirect;
}


int
get_length(char *input) {
    int index, size;
    int length;

    length = strlen(input);
    size = 0;
    
    for (index = 0; index < length; index++) {
        if (input[index] == '>' || input[index] == '<' || input[index] == '|') {
            size += 3;
        } else {
            size++;
        }
    }

    return size;
}

char* parse_input_string(char* input){
    char *output;
    int index, token_index;

    int estimated_length = get_length(input);
    int input_length = strlen(input);

    if ((output = malloc(estimated_length + 1)) == NULL) {
        return NULL;
    }

    token_index = 0;

    for (index = 0; index < input_length; index++) {
        if (input[index] == '>' || input[index] == '<' || input[index] == '|') {
            output[token_index] = ' ';
            token_index++;
            output[token_index] = input[index];
            token_index++;
            if (input[index] == '>') {
                if ((input_length - index) > 1) {
                    if (input[index + 1] == '>') {
                        output[token_index] = input[index];
                        token_index++;
                        index++;
                    }
                }
            }
            output[token_index] = ' ';
            token_index++;
        } else {
            output[token_index] = input[index];
            token_index++;
        }
    }
    output[token_index] = '\0';
    return output;
}
/* call in case of redirect commands that is when we have >, >> or < in the commands */
int redirect_commands(char *commands[],char* duplicate[],int command_length,bool background){
    for(int i=0;i<command_length;i++){
        if (commands[i+1] == NULL) {
            perror("Insufficient Arguement ERROR: \n");
            return EXIT_FAILURE;
        }
        if(strcmp("<", commands[i]) == 0) {
            if ((commands[i+2] != NULL ) && (commands[i+ 3] != NULL) && (strcmp(">", commands[i+1 + 1]) == 0)) {
                execute_with_execvp(duplicate, commands[i+1], commands[i+1 + 2], false, background,true);
                return  EXIT_SUCCESS;
            }else if ((commands[i+2] != NULL) && (commands[i+3] != NULL) && (strcmp(">>", commands[i+1 + 1])) == 0){
                execute_with_execvp(duplicate, commands[i+1], commands[i+3], true, background,true);
                return EXIT_SUCCESS;
            }else{
                execute_with_execvp(duplicate, commands[i+1], NULL, false, background,true);
                return  EXIT_SUCCESS;
            }
        }
        if ((strcmp(">", commands[i]) == 0) && commands[i+1]!=NULL) {
            
            execute_with_execvp(duplicate, NULL, commands[i+1], false, background,true);
            return EXIT_SUCCESS;
        }
        if ((strcmp(">>", commands[i]) == 0) && commands[i+1] != NULL) {
            
            execute_with_execvp(duplicate, NULL, commands[i+1], true, background,true);
            return  EXIT_SUCCESS;
        }
    }
    return EXIT_FAILURE;
}