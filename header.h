
//parsing functions (parse.c)
char **tokenize(char *line);
char*** checkSemiColon(char** tokens, int tokenLength);
char*** checkAmpersand(char** tokens, int tokenLength);
char*** parseRedirect(char** tokens, int tokenLength);

//functions for executing the processes in terminal (funclist.c)
void ChangeDir(char* a);
void funcList(char** tokens, int tokenLength, int background);

//functions to find length of array(funclist.c)
int findLength(char** tokens);
int findLength2(char*** tokens);

//Just to remove the warning as gets() is deprecated
char *gets(char *s);