#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/wait.h>


#include "header.h"

#define MAX_BACKGROUND_PROCESSES 64

//Finding the the length of an array of strings
int findLength(char** tokens){
    int count = 0;
    for(int i=0;tokens[i]!=NULL;i++){
            count++;
        }
    return count;
}

//Finding the the length of an array of array of strings
int findLength2(char*** tokens){
    int count = 0;
    for(int i=0;tokens[i]!=NULL;i++){
            count++;
        }
    return count;
}

//for changing the directory
void ChangeDir(char* a){

    int s = chdir(a);
    if(s == -1){
        printf("mosh: No such file or directory\n");
    }
}

/*
    List of functions which will be executed
    tokens is a single command. 
    background denotes whether the function will run in background or not
    background = 0 => process is running in foreground
    background = 1 => process is running in background
*/
pid_t funcList(char** tokens, int tokenLength, int background){

    //If an empty command is entered, the shell shouldn't be crashing
    if(tokenLength == 0) return -1;

    //Checking whether redirect operation is hidden in tokens
    char*** parseres = parseRedirect(tokens,tokenLength);

    //length of parseres being 1 implies that tokens is a single command
    if(findLength2(parseres) == 1){

        //Change directory function.
        if(strcmp(tokens[0],"cd") == 0){
             
            //Ensuring exactly one argument is provided to "cd"
            if(tokenLength == 2){
                ChangeDir(tokens[1]);
            }
            else if(tokenLength == 1){

                printf("cd: missing operand\n");
            }
            else{
                printf("cd: too many arguments provided\n");
            }
            return -1;
        }
        //tokens which will be gobbled up by the execvp command of a child process
        else{

            pid_t pid = fork(); //fork

            if(pid == 0){
                //Inside child process

                //Set process group id for background process
                if(background){
                    setpgid(0,0);
                }

                //executing the command
                int res = execvp(tokens[0],tokens);
                
                if(res == -1){//Catches incorrect commands
                    printf("%s: command not found\n",tokens[0]);
                }

                exit(0);
            }
            else{
                //recall that background = 0 means the process is running in foreground
                if(!background) waitpid(pid,NULL,0);
                return pid;
            }
        }
    }
    else{

        pid_t pid = fork(); //fork

        if(pid == 0){
            //Inside child process

            //Set process group id for background process
            if(background){
                setpgid(0,0);
            }
            //Closing standard output of child process.
            close(1);

            /*
                parseres[1][0] contains the file name
                parseres[0][0] contains echo command (or an equivalent command) 
            */
            int fd = open(parseres[1][0],O_RDWR | O_CREAT,00777);
            
            int res = execvp(parseres[0][0],parseres[0]);
            
            if(res == -1){//Catches incorrect commands
                    printf("%s: command not found\n",tokens[0]);
            }

            close(fd);

            exit(0);
        }
        else{
            //recall that background = 0 means the process is running in foreground
            if(!background) waitpid(pid,NULL,0);
            return pid;
        }

    }

}
