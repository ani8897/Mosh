#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/wait.h>


#include "header.h"

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
    background = 1 => process is running in foreground
    background = 2 => process is running in background
*/
void funcList(char** tokens, int tokenLength, int background){

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
        }
        //tokens which will be gobbled up by the execvp command of a child process
        else{

            pid_t pid = fork(); //fork

            if(pid == 0){
                //Inside child process

                //executing the command
                int res = execvp(tokens[0],tokens);
                if(res == -1){//Catches incorrect commands
                    printf("%s: command not found\n",tokens[0]);
                }


                if(background == 2){
                    printf("mosh: background process (pid: %d) completed\n",getpid());
                }
                exit(0);
            }
            else{
                //int s = setpgid(pid, background);
                //printf("%d\n", s);

                //recall that background = 1 means the process is running in foreground
                if(background == 1) waitpid(pid,NULL,0);
            }
        }
    }
    else{

        pid_t pid = fork(); //fork

        if(pid == 0){
            //Inside child process

            //Closing standard output of child process.
            close(1);

            /*
                parseres[1][0] contains the file name
                parseres[0][0] contains echo command (or an equivalent command) 
            */
            int fd = open(parseres[1][0],O_RDWR | O_CREAT,00777);
            
            int res = execvp(parseres[0][0],parseres[0]);
            printf("%d\n", res);
            close(fd);

            if(background == 2){
                    printf("mosh: background process (pid: %d) completed\n",getpid());
            }
            exit(0);
        }
        else{
            setpgid(pid, (pid_t) background);
            //recall that background = 1 means the process is running in foreground
            if(background == 1) waitpid(pid,NULL,0);
        }

    }

}
