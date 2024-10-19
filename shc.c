/***************************************************************************//**

  @file         main.c

  @author       Hasnain Merchant

  @date         Saturday,  19 October 2024

  @brief        SCH (Shell in C)

*******************************************************************************/
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h> //chdir(), fork(), exec(), pid_t
#include <stdlib.h> //malloc(), realloc(), free(), exit(), execvp(), EXIT_SUCCESS, EXIT_FAILURE
#include <stdio.h> //fprintf(), printf(), stderr, getchar(), perror()
#include <string.h> //strcmp(), strtok()

//Creating Shell Builtins (cd, help, exit)
/* Function Decalrations for builtin shell Commands*/
int sch_cd(char **args);
int sch_help(char **args);
int sch_exit(char **args);

/*List of builtin commands, followed by corresponding functions*/

char *builtin_str[] = {
    "cd",
    "help",
    "exit"
};

/*Array of function pointers
Each function takes a char ** (pointer to a pointer to a char) as an argument and returns an int
The array is initialized with the address of the function e.g sch_cd. This means sch_cd is a function that matches 
the signature int function_name(char **)
*/
int (*builtin_func[]) (char **) = {
    &sch_cd,
    &sch_help,
    &sch_exit
};

int sch_num_builtins() {
    return sizeof(builtin_str) / sizeof(char *);
}

//Function Implementations
int sch_cd(char **args) {
    if(args[1] == NULL) {
        fprintf(stderr, "sch: expected argument to \"cd\"\n");
    } else {
        if (chdir(args[1]) != 0) {
            perror("SCH");
        }
    }
    return 1;
}

int sch_help(char **args) {
    int i;
    printf("Hasnain Merchant's SCH\n");
    printf("Type program names and arguments, and hit enter.\n");
    printf("The following are built in:\n");

    for(i=0;i<sch_num_builtins();i++) {
        printf("   %s\n", builtin_str[i]);
    }

    printf("Use the man command for information on other programs.\n");
    return 1;
}

int sch_exit(char **args) {
    return 0;
}

//Putting togther builtins and processes
int sch_execute(char **args) {
    int i;
    if(args[0] == NULL) {
        return 1;
    }

    for(i = 0;i<sch_num_builtins();i++) {
        if(strcmp(args[0], builtin_str[i] == 0)) {
            return (*builtin_func[i])(args);
        }
    }
    return sch_launch(args);
}

//Starting Shell Processes
int sch_launch(char **args) {
    pid_t pid;
    int status;

    pid = fork();
    if(pid == 0){
        //Child Process
        if(execvp(args[0], args) == -1) {
            perror("SCH");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        //Error forking
        perror("SCH");
    } else {
        //Parent Process
        do {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 1;
}

//Parsing The Line
#define SHC_TOK_BUFSIZE 64
#define SHC_TOK_DELIM " \t\r\n\a"
char **shc_split_line(char *line){
    int buffer_size = SHC_TOK_BUFSIZE, pos = 0;
    char **tokens = malloc(buffer_size * sizeof(char*));
    char *token, **tokens_backup;

    if(!tokens) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, SHC_TOK_DELIM);
    while (token != NULL) {
        tokens[pos] = token;
        pos++;
        if(pos >= buffer_size) {
            buffer_size += SHC_TOK_BUFSIZE;
            tokens_backup = tokens;
            tokens = realloc(tokens, buffer_size * sizeof(char*));
            if(!tokens) {
                free(tokens_backup);
                fprintf(stderr, "lsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, SHC_TOK_DELIM);
    }
    tokens[pos] = NULL;
    return tokens;
}

//Reading A Line
char *sch_read_line(void) {
    char *line = NULL;
    ssize_t buffersize = 0; //have getline allocate buffer for us

    if(getline(&line, &buffersize, stdin) == -1) {
        if(feof(stdin)) {
            exit(EXIT_SUCCESS); //received and EOF
        } else {
            perror("sch: getline\n");
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
        if(c == EOF){
            exit(EXIT_SUCCESS);
        } else if (c == '\n') {
            buffer[pos] = '\0';
            return buffer;
        } else {
            buffer[pos] = c;
        }
        pos++;

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
        printf(">> ");
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