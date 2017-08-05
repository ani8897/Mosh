#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include "header.h"

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64

//For tokenizing a line fetched from the terminal
char **tokenize(char *line)
{
    char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
    char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
    int i, tokenIndex = 0, tokenNo = 0;

    for(i =0; i < strlen(line); i++){

        char readChar = line[i];

        if (readChar == ' ' || readChar == '\n' || readChar == '\t') {
            token[tokenIndex] = '\0';
            if (tokenIndex != 0){
                tokens[tokenNo] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));
                strcpy(tokens[tokenNo++], token);
                tokenIndex = 0; 
            }
        } 
        else {
            token[tokenIndex++] = readChar;
        }
    }
 
    free(token);
    tokens[tokenNo] = NULL ;
    return tokens;
}

//Splitting the commands by parsing for ";;"

char*** checkSemiColon(char** tokens, int tokenLength){

    char ***res = (char ***)malloc(MAX_NUM_TOKENS * sizeof(char **));
    
    //storing a command temporarily until ";;" is encountered
    char **temp = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));

    int i, tokenNo = 0, command = 0;

    //Idea similar to that used in tokenise
    for(i=0; i<tokenLength; i++){

        if(strcmp(tokens[i],";;") == 0){
            temp[tokenNo] = NULL;
            res[command++] = temp;
            temp = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
            tokenNo = 0;
        }
        else{
            temp[tokenNo++] = tokens[i];
        }
    }

    res[command++] = temp;
    res[command] = NULL;
    return res;
}

//Similar to checkSemiColon, just replacing ";;" with "&&"
char*** checkAmpersand(char** tokens, int tokenLength){
    char ***res = (char ***)malloc(MAX_NUM_TOKENS * sizeof(char **));
    char **temp = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
    int i, tokenNo = 0, command = 0;

    for(i=0; i<tokenLength; i++){

        if(strcmp(tokens[i],"&&") == 0){
            temp[tokenNo] = NULL;
            res[command++] = temp;
            temp = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
            tokenNo = 0;
        }
        else{
            temp[tokenNo++] = tokens[i];
        }
    }
    res[command++] = temp;
    res[command] = NULL;
    return res;
}

// Parsing for '>'. Assuming only one '>' is provided
char*** parseRedirect(char** tokens, int tokenLength){

    char ***res = (char ***)malloc(MAX_NUM_TOKENS * sizeof(char **));
    char **temp = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
    
    int i, tokenNo = 0, command = 0;
    int notFound = 1;

    for(i=0 ; i<tokenLength; i++){

        if(strcmp(tokens[i],">") == 0){
            temp[tokenNo] = NULL;
            res[command++] = temp;
            temp = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
            tokenNo = 0;
        }
        else{
            temp[tokenNo++] = tokens[i];
        }
    }
    res[command++] = temp;
    res[command] = NULL;
    return res;
}
