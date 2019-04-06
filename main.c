#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#define line_size 1024
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_RESET "\x1b[0m"
#define SIGINT  2   /* Interrupt the process */

void failure_message (void)
{
    perror("the following error occured");
    printf("Value of errno: %d\n",errno);
    exit(EXIT_FAILURE);
}


void create_directory(char **tokens)
{
    int temporary;
    if ( (temporary=mkdir(tokens[1],0777))==-1)
        failure_message();

}
void system_comment(char** tokens)
{
    pid_t pid =fork();
    if(pid ==-1)
    {
        failure_message();
    }
    else if (pid ==0)
    {
        if( execvp(tokens[0],tokens) ==-1)
        {
            failure_message();
        }
    }
    else
    {

        wait(NULL);
    }
}
void goodbye(void)
{
    printf(ANSI_COLOR_BLUE"goodbye\n"ANSI_COLOR_RESET);
    exit(EXIT_SUCCESS);
}
int change_directory(char** tokens)
{
    if (tokens[1]==NULL)
    {
        printf(ANSI_COLOR_RED"cd need argument\n"ANSI_COLOR_RESET);
    }
    else
    {
        if (chdir(tokens[1])==-1)
        {
            printf("no directory with given tittle, your current working directory remain unchanged\n");

        }
    }
}
void help_info()
{
    printf(ANSI_COLOR_MAGENTA"my name is Krzysztof Ignasiak\n");
    printf("this microshell is a form of assigment of Operating System Program\n");
    printf("it shows information about user login,user uid,user hostname and current working directory\n");
    printf("it can interpret and evaluate built_in commands (help,exit,cd,touch,mkdir) or other scripts using system commands like fork,exec and wait\n");
    printf("it contains functions,which will inform user, when the error occurs\n");
    printf("User can close program using Ctrl-c \n"ANSI_COLOR_RESET);
}
void print_basic_information(void)
{
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    char hostname[1024];
    hostname[1023] = '\0';
    gethostname(hostname, sizeof(hostname));
    printf(ANSI_COLOR_GREEN"%s:%d:%s:%s"ANSI_COLOR_RESET,getlogin(),getuid(),hostname,cwd);
}
int memory_alloc_check(char *memory)
{
    if (memory ==NULL)
    {

        return 0;
    }
    else
    {
        return 1;
    }
}
int built_in_tools(char** tokens)
{  //declaring
    int number_of_tools =4;
    int number_of_case =0;
    char ** tools = malloc(number_of_tools* sizeof(char*));

    //checking memory allocation
    if(tools ==NULL)
    {
        failure_message();
    }


    //list of built-in fuctions
    tools[0]="cd";
    tools[1]="help";
    tools[2]="exit";
    tools[3]="mkdir";



    //checking which case is true


    for (int i = 0; i < number_of_tools;++i)
    {
       if(strcmp(tokens[0],tools[i])==0) {
            number_of_case = i+1;
           break;
        }
    }

    //checking cases
    switch (number_of_case)
    {
        case 1:
            change_directory(tokens);
            return 1;

        case 2:
            help_info();
            return 1;
        case 3:
            goodbye();
            return 1;
        case 4:
            create_directory(tokens);
            return 1;

        default:
            return 0;

    }
}


void arguments(char* user_line, int characters_counters)
{    int builtin;
    char * token;
    char **tokens = malloc(characters_counters* sizeof(char*));
    if(tokens==NULL)
    {
        failure_message();
    }
    token=strtok(user_line," ");
    if(token==NULL)
    {
        failure_message();
    }
    else
    {
        tokens[0]=token;
    }

    for (int i = 1; i <characters_counters ; ++i) {
        token=strtok(NULL," ");
        if(token == NULL)
        {
            break;
        }
        else
        {
            tokens[i]=token;
        }
    }
   builtin= built_in_tools(tokens);
    if (builtin ==0)
    {
        system_comment(tokens);
    }

    free(user_line);
    free(tokens);
}

char* read_line(void) {
    //initialize
    int userline_size = line_size;
    char *line_from_user;
    int character_counters = 0;
    int character;

    //allocating memory
    line_from_user = calloc(userline_size, sizeof(char));
    if (memory_alloc_check(line_from_user)==0)
    {
        failure_message();
    }

    //scaning
    //user_input_loop(&userline_size,line_from_user,character_counters);

    while (1) {
        if (character_counters >= userline_size) {
            userline_size += line_size;
          line_from_user = realloc(line_from_user, userline_size);

               if( memory_alloc_check(line_from_user)==0)
               {
                   failure_message();
               }

        }
       character = fgetc(stdin); //getting char

       if (character == '\n' || character == EOF) {  // wating for end
            line_from_user[character_counters] = '\0';
            //return line_from_user;
           break;
       } else {
           line_from_user[character_counters] = character;  //put into array
           character_counters++;  // increase amount
        }
    }
   arguments(line_from_user,character_counters);
}
void handle_sigint(int sig)
{
    goodbye();
}
int main() {
   while (1) {
       signal(SIGINT,handle_sigint);
       print_basic_information();
       printf(" $ ");

       read_line();
   }
    return 0;
}