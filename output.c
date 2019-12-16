#include "sish.h"

int
get_length(char *input); 

int
main(int argc, char **argv) {
    char *input = "ls>file1>>file2<file3 > file5|fil6 |file7";
    char *output;
    int index, token_index;

    int estimated_length = get_length(input);
    int input_length = strlen(input);

    if ((output = malloc(estimated_length + 1)) == NULL) {
        return 1;
    }

    output[0] = '\0';
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

    printf("output: %s\n", output);    
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