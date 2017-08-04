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

//Beautification of the shell
#define KGRN  "\x1B[32m"
#define KCYN  "\x1B[36m"
#define BSTRT "\033[1m"
#define BEND "\033[0m"

// https://stackoverflow.com/questions/7171722/how-can-i-handle-sigchld-in-c
// https://stackoverflow.com/questions/3585846/color-text-in-terminal-applications-in-unix

//Handling interrupts
void sigHandler(int signum) {
    printf("\n");
    signum = 0;
    return;
}

void main(void)
{
    char  line[MAX_INPUT_SIZE];            
    char  **tokens;              
    int i, tokenLength;

    //Interrupt Handler
    signal(SIGINT,sigHandler);

    //Infinite loop for the running the shell
    while (1) {           
       
        //Shows the current directory 
        char cwd[1024];      
        printf("%s%smosh:%s%s%s$>",BSTRT,KGRN,KCYN,getcwd(cwd,sizeof(cwd)),BEND);     
        
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
                funcList(commandlist[i],findLength(commandlist[i]),1);
            }  
        }
        else{    

           for(int i = 0; i<commandlength; i++){
                funcList(commandlist[i],findLength(commandlist[i]),2);
            }        
       }

        // Freeing the allocated memory	
        for(i=0;tokens[i]!=NULL;i++){
            free(tokens[i]);
        }
        free(tokens);
    }
     

}

                
/*
        for(i=0;tokens[i]!=NULL;i++){
           printf("found token %s\n", tokens[i]);
        }*/

        // for(int i = 0; i<findLength2(checkSemiColon); i++){
        //     //funcList(commandlist[i],findLength(commandlist[i]));
        //     for(int j=0; j<findLength(checkSemiColon[i]); j++){
        //         printf("%s", checkSemiColon[i][j]);
        //     }
        //     printf("\n");
        // } 