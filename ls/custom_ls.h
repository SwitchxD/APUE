#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdio.h>
int converting_array(char *str, char *sep, char *** list);
void freeing_array(char ** list, int size);
void printing_array(char **list, int size);
void getting_path();
void readline(char *input);
int checking_implementing(char *filepath);
int finding_path(char *cmd, char *filepath);
int posting_array(char * str, char ** list, int size);
void executing_path(int argslen, char * cmd, char ** args, char * filepath);
void redirfd(int redirfd, char *pathname, int flags, mode_t mode);
void child(int argslen, char * cmd, char ** args, char * filepath);
int verifyampersand(char ** inputlist, int size);
void executing_command(char *cmd, char **args, int argslen, char *filepath);
void handling_command(char *cmd, char **args, int argslen);
