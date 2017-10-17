#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAXINPUT 200

int main(int argc, char **argv)
{
    /*create arrays to store shell prompt, user input, directory, variable and value*/
    char* prompt = "308sh> ";

    char input[MAXINPUT];
    char direct[MAXINPUT];
    char variable[MAXINPUT];
    char value[MAXINPUT];
    int status;
    int child1;
    int child2;
    char cwd[1024];
    int backgroundProcess = 0;
    char* backgroundCommand;
    /*counters*/
    int i, k;
    int numArguments;
    
    /*Intialization*/
    
    /*check for number of arguments. Exit if too many or too less*/
    if(argc != 3 && argc != 1)
    {
        printf("Invalid command! Exiting.\n");
        return 0;
    }
    /*check for prompt command*/
    if(argc == 3)
    {
        /*check if the first argument is -p*/
        if((strlen(argv[1]) != 2) || (strncmp("-p", argv[1], 2) != 0))
        {
            printf("Invalid command! Exiting.\n");
            return 0;
        }
        prompt = argv[2];
    }
    
    /*Input*/
    
    while(1)
    {
        /*print '308sh>' and wait for input*/
        printf(prompt);
        
        /*check input as string*/
        fgets (input, MAXINPUT, stdin);
        if ((input[strlen (input) - 1] == '\n') && (strlen(input)>0))
        {
            input[strlen(input) - 1] = '\0';
        }
        
        /*Buildin command checking*/
        
        /*exit – the shell should terminate and accept no further input from the user*/
        if((strlen(input) == 4) && (strncmp("exit", input, 4) == 0))
        {
            return 0;
        }
        
        /*pid – the shell should print its process ID*/
        else if((strlen(input) == 3) && (strncmp("pid", input, 3) == 0))
        {
            printf("Process ID of the shell is: %d", getpid());
            printf("\n");
        }
        
        /*ppid – the shell should print the process ID of its parent*/
        else if((strlen(input) == 4) && (strncmp("ppid", input, 4) == 0))
        {
            printf("Process ID of shell's parent is: %d", getppid());
            printf("\n");
        }
        
        /*cd <dir> – change the working directory. With no arguments, change to the user’s home directory
         (which is stored in the environment variable HOME)*/
        else if(strncmp("cd", input, 2) == 0 && strlen(input) == 2)//if only 'cd' has been inputed, go back to home directory
        {
          
   
                chdir(getenv("HOME"));/* chdir() changes the current working directory of the calling process to the
                                       directory specified in path.
                                       getenv() function searches the environment list to find the
                                       environment variable name, and returns a pointer to the corresponding
                                       value string.*/
         }
        else if(strncmp("cd", input, 2) == 0 && input[2] == ' ')//cd follows by directory
        {
                for(i = 0; i < strlen(input) - 3; i++)
                {
                    direct[i] = input[i + 3];
                    direct[i+1] = '\0';
                }
                if(chdir(direct) == -1)//can not find directory
                {
                    printf("Directory cannot be found\n");
                }
        }
        
        /*pwd – print the current working directory*/
        else if((strlen(input) == 3) && (strncmp("pwd", input, 3) == 0))
        {
            if(getcwd(cwd, 1024) == -1)/*getcwd() function copies an absolute pathname of the current
                                        working directory to the array pointed to by buf, which is of length
                                        size.*/
            {
                printf("Can't print current working directory");
            }
            printf("%s\n", cwd);
        }
        
        /*set <var> <value> – sets an environment variable (which is visible in all future child processes).
         If there is only one argument, clears the variable.*/
        else if(strncmp("set", input, 3) == 0 && input[3] == ' ')
        {
            /*initialize k: at end of for loop k < -1 fails, k = -1 clears environment variable, k > -1 sets environment variable*/
            k = -2;
            for(i=4; i < strlen(input); i++)
            {
                if(k == -2 && input[i] != ' ')
                {
                    k = -1;
                }
                if(input[i] == ' ')
                {
                    if(k != -1 || i == strlen(input) - 1)
                    {
                        printf("Invalid number for set argument\n");
                        k = -3;
                    }
                    else
                    {
                        k=0;
                    }
                }
                /*if k = -1 we are filling the variable string*/
                else if(k == -1)
                {
                    variable[i-4] = input[i];
                }
                /*if k is > -1 we are filling the variable string*/
                else
                {
                    value[k] = input[i];
                    k++;
                }
            }
            if(k == -2)
            {
                printf("Invalid number for set argument\n");
            }
            else if (k == -1)
            {
                if(setenv(variable, NULL, 1) == -1)/*setenv() function adds the variable name to the environment with
                                                    the value value, if name does not already exist.  If name does exist
                                                    in the environment, then its value is changed to value if overwrite
                                                    is nonzero; if overwrite is zero, then the value of name is not
                                                    changed (and setenv() returns a success status). */
                {
                    printf("Failed to clear environment variable\n");
                }
            }
            else
            {
                if(setenv(variable, value, 1) == -1)
                {
                    printf("Failed to set environment variable\n");
                }
            }
        }
        
        /*get <var> – prints the current value of an environment variable*/
        else if(strncmp("get", input, 3) == 0 && input[3] == ' ')
        {
            for(i = 0; i < strlen(input - 4); i++)
            {
                variable[i] = input[i+4];
            }
            if(getenv(variable) == NULL)
            {
                printf("Failed to find environment variable\n");
            }
            else
            {
                printf("%s\n", getenv(variable));
            }
        }
        
        /*Executable command*/
        
        else
        {
            /*count number of arguments*/
            numArguments = 1;
            for(i = 0; i < strlen(input); i++)
            {
                if (input[i] == ' ')
                {
                    numArguments++;
                }
            }
            /*check for '&' character and set as backgroundProcess*/
            if(input[strlen(input) - 1] == '&')
            {
                numArguments--;
                backgroundProcess = 1;
            }
            /*use temp string x as individual argument to fill args[]*/
            char x[numArguments][MAXINPUT];
            char* args[numArguments + 1];
            numArguments = 0;
            k = 0;
            for(i = 0; i < strlen(input) + 1; i++)
            {
                if (input[i] == ' ' || input[i] == '\0')
                {
                    x[numArguments][k] = '\0';
                    args[numArguments] = x[numArguments];
                    numArguments++;
                    k = 0;
                }
                else if (input[i] == '&' && i == strlen(input) - 1)
                {
                    i = strlen(input) + 1;
                }
                else
                {
                    x[numArguments][k] = input[i];
                    k++;
                }
            }
            args[numArguments] = (char*) NULL;
            
            /*check for child process*/
            if(backgroundProcess == 1)
            {
                backgroundProcess = 0;
                child2 = fork();
                if(child2 == 0)
                {
                    child1 = fork();
                    if(child1 == 0)
                    {
                        printf("[%d] %s\n", getpid(), args[0]);
                        execvp(args[0], args);
                        perror("\0");           /*perror() function produces a message on standard error describing
                                                 the last error encountered during a call to a system or library
                                                 function.*/
                        return 0;
                    }
                    else
                    {
                        status = -1;
                        waitpid(-1, &status, 0);
                        if(1)
                        {
                            printf("\n[%d] %s Exit %d\n", child1, args[0], WEXITSTATUS(status));/*WEXITSTATUS(wstatus)
                                                                                                 returns the exit status of the child.  This consists of the
                                                                                                 least significant 8 bits of the status argument that the child
                                                                                                 specified in a call to exit(3) or _exit(2) or as the argument
                                                                                                 for a return statement in main().  This macro should be
                                                                                                 employed only if WIFEXITED returned true.*/
                            printf(prompt);
                        }
                        else
                        {
                            printf("[%d] %s Exit %d\n", child1, args[0], WEXITSTATUS(status));
                        }
                        return 0;
                    }
                }
                else
                {
                    usleep(1000);
                }
            }
            /*run child's command and wait for it to finish, then take futher input*/
            else
            {
                child1 = fork();
                /*child process*/
                if(child1 == 0)
                {
                    printf("[%d] %s\n", getpid(), args[0]);
                    execvp(args[0], args);
                    perror("\0");
                    return 0;
                }
                /*parent process*/
                else
                {
                    usleep(1000);
                    status = -1;
                    waitpid(child1, &status, 0);
                    printf("[%d] %s Exit %d\n", child1, args[0], WEXITSTATUS(status));
                }
            }
        }
    }
}
