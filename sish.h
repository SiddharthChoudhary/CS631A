#include <sys/wait.h>

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>

#define _SISH_
#define DFTPERMISSION 0644
#define REDIRECTION_APPEND 1
#define BUF_SIZE 1024
#define SISH_ERROR "sish [ −x] [ −c command]\n"
#define X_FLAG_OUTPUT "+ "
#define MAXTOKENS 128
#define MAX_LENGTH_COMMAND 1024


extern int x_flag;
extern int c_flag;

char* setFlags(int argc,char** argv);
void setEnv_SHELL();
void flow_if_c_flag_is_on(char* command);
void flow_if_otherwise();
int execute_commands(int num,char *commands[]);
int execute_pipe(char *commands[],int length_token);
int call_builtin_cd_exit_echo(int number_of_commands,char *commands[]);
int redirect_commands(char *commands[],char* duplicate[],int number_of_commands,bool background);
bool check_if_redirect_io_and_create_a_copy(char *commands[],char *duplicate[]);
int execute_with_execvp(char* commands[],char* input,char* output,bool append, bool background_process,bool redirection);
int redirect_to_std_input_output_from_fd(char* input,char* output,bool append);
char* parse_input_string(char* input);
int get_length(char *input);