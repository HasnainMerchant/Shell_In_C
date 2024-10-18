#include <unistd.h> //chdir(), fork(), exec(), pid_t
#include <stdlib.h> //malloc(), realloc(), free(), exit(), execvp(), EXIT_SUCCESS, EXIT_FAILURE
#include <stdio.h> //fprintf(), printf(), stderr, getchar(), perror()
#include <string.h> //strcmp(), strtok()

//Reading A Line
char *sch_read_line(void) {
    char *line = NULL;
    ssize_t buffersize = 0; //have getline allocate buffer for us

    if(getline(&line, &buffersize, stdin) == -1) {
        if(feof(stdin)) {
            exit(EXIT_SUCCESS); //received and EOF
        } else {
            perror("readline");
            exit(EXIT_FAILURE);
        }
    }
    return line;
}

//Reading a Line (oldway)
#define BUFSIZE 1024
char *sch_read_line_old(void) {
    int buffer_size = BUFSIZE;
    int pos = 0;
    char *buffer = malloc(sizeof(char) * buffer_size);
    int c;

    if(!buffer) {
        fprintf(stderr, "sch: allocation error\n");
        exit(EXIT_FAILURE);
    }

    while(1) {
        //Read a character
        c = getchar();

        //If we hit EOF, replcae it with a null character and return.
        if(c == EOF || c == '\n'){
            buffer[pos] = '\0';
            return buffer;
        } else {
            buffer[pos++] = c;
        }

        //If we have exceeded the buffer, reallocate
        if(pos >= buffer_size) {
            buffer_size += BUFSIZE;
            buffer = realloc(buffer, buffer_size);
            if(!buffer) {
                fprintf(stderr, "sch: reallocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}

//Basic Loop Of A Shell
void sch_loop(void) {
    char *line;
    char **args;
    int status;

    do {
        printf("$ ");
        line = sch_read_line();
        args = sch_split_line(line);
        status = sch_execute(args);

        free(line);
        free(args);
    } while (status);
}

//The Basic lifetime of a shell
int main (int argc, char **argv) {
    //Load config files, if any
    
    // Run command loop
    sch_loop();
    //Perform any shutdown/cleanup

    return EXIT_SUCCESS;
}