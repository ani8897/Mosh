#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/wait.h>

#include <signal.h>

#include "header.h"

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64
#define MAX_BACKGROUND_PROCESSES 64

//Beautification of the shell
#define KGRN  "\x1B[32m"
#define KCYN  "\x1B[36m"
#define BSTRT "\033[1m"
#define BEND "\033[0m"


//for maintaining the background processes           
pid_t bg_processes[MAX_BACKGROUND_PROCESSES];

//for detecting whether SIGINT was sent to a foreground process
int fg_kill = 0;
            
//Handling interrupts
void sigHandler(int signum) {
    
    if(signum == SIGINT){
        printf("\n");
        fg_kill = 1; //Setting fg_kill to prevent execution of further processes in foreground
        return;
    }

    if(signum == SIGCHLD){
        int status;
        pid_t pid;

        while( (pid = waitpid(-1, &status, WNOHANG)) > 0){
            //printf("pid: %d\n", pid);
            for(int i = 0; i<MAX_BACKGROUND_PROCESSES; i++){
                //printf("%d", bg_processes[i]);
                if(bg_processes[i] == pid){
                    //printf("Removing pid: %d\n", bg_processes[i]);
                    bg_processes[i] = -1;
                }
            }
            printf("Background process completed\n");
        }
    }    
}

void main(void)
{
    char  line[MAX_INPUT_SIZE];            
    char  **tokens;   
    int tokenLength;

    for(int i = 0; i<MAX_BACKGROUND_PROCESSES; i++){   
        bg_processes[i] = -1;
        //printf("Initialising\n");
    }
    //Interrupt Handler
    signal(SIGINT,sigHandler);
    signal(SIGCHLD,sigHandler);

    //Infinite loop for the running the shell
    while (1) {           
       
        //Shows the current directory 
        char cwd[1024];      
        printf("%s%smosh:%s%s%s$>",BSTRT,KGRN,KCYN,getcwd(cwd,sizeof(cwd)),BEND);     
        //printf("mosh:%s$>",getcwd(cwd,sizeof(cwd)));     
        
        //Obtain the line in the form of tokens
        bzero(line, MAX_INPUT_SIZE);
        gets(line);           
        line[strlen(line)] = '\n'; //terminate with new line
        tokens = tokenize(line);

        //Finding the number of tokens
        tokenLength = findLength(tokens);

        //If no input has been entered then just continue with the while loop
        if(tokenLength == 0) continue;
        
        //Exit with the child processes killed (partially implemented)
        if(strcmp(tokens[0],"exit") == 0){
            for(int i = 0; i<MAX_BACKGROUND_PROCESSES; i++){
                if (bg_processes[i]!=-1) {
                    //printf("Killing pid: %d\n", bg_processes[i]);
                    kill(bg_processes[i], SIGINT);
                }
            }
            break;
        }
        
        //Checking for background processes
        char*** commandlist = checkAmpersand(tokens,tokenLength);
        
        //The number of commands to be executed
        int commandlength = findLength2(commandlist);
        if(commandlength==1){
            commandlist = checkSemiColon(tokens,tokenLength);
            commandlength = findLength2(commandlist);
 
            for(int i = 0; i<commandlength; i++){
                int dump = funcList(commandlist[i],findLength(commandlist[i]),0);
                
                //If SIGINT is pressed during the execution of sequential processes, then break off.
                if(fg_kill) break;
            }  
        }
        else{    

           for(int i = 0; i<commandlength; i++){
                //Obtaining the background child process pid and storing it in the array
                int bgpid = funcList(commandlist[i],findLength(commandlist[i]),1);
                int flag = 1;
                for(int i = 0; i<MAX_BACKGROUND_PROCESSES; i++){
                    if(bg_processes[i] == -1){
                        flag = 0;
                        bg_processes[i] = bgpid;
                        //printf("Adding pid: %d \n", bg_processes[i]);
                        break;
                    }
                }
                if(flag){
                    printf("mosh: too many background processes running\n");
                }
            }        
       }

        // Freeing the allocated memory	
        for(int i=0;tokens[i]!=NULL;i++){
            free(tokens[i]);
        }
        free(tokens);

        //Resetting the foreground process kill variable
        fg_kill = 0;
    }
     

}

                

//https://stackoverflow.com/questions/31073999/text-auto-complete-using-tab-in-command-line
