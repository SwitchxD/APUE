#include "custom_ls.h"
#define MAX 100

int number_of_paths;
char *path_default = "/bin:.";
char ** actual_path;


int main(int argc,char * argv[])
{
  getting_path();
  char input[MAX];
  char **inputlist;
  int size;
  int itr=0;
  while(itr<1){
    strcpy(input,"ls ");
    if(argc==1){
      strcpy(input,"ls");
    }
    for(int i=1;i<argc;i++){
        strcat(input,argv[i]);
        if(i!=argc-1)
            strcat(input," ");
    }
    size = converting_array(input, " ", &inputlist);
    handling_command(inputlist[0], inputlist+1, size-1);
    freeing_array(inputlist, size);
    itr++;
  }
  freeing_array(actual_path, number_of_paths);
  return 0;
}
int converting_array(char *str, char *sep, char *** list)
{
  int len = strlen(str);
  char *copystr = (char *) malloc((len + 1) * sizeof(char));
  strcpy(copystr, str);
  int sizelist = 0;
  char *cur, *temp;
  temp = copystr;
  while( ( cur = strsep(&temp, sep) ) )
  {
    sizelist++;
  }
  (*list) = (char **) malloc(sizelist * sizeof(char*));
  int i = 0;
  int curlen;
  strcpy(copystr, str);
  temp = copystr;
  while( ( cur = strsep(&temp, sep) ) )
  {
    curlen = strlen(cur);
    (*list)[i] = (char *) malloc((curlen + 1) * sizeof(char));
    strcpy((*list)[i], cur);
    i++;
  }
  free(copystr);
  return sizelist;
}
void freeing_array(char ** list, int size)
{
  int i;
  for(i = 0; i<size; i++)
  {
    free(list[i]);
  }
  free(list);
}
void printing_array(char **list, int size)
{
  int i;
  for(i = 0; i<size; i++)
  {
    printf("%s\n",list[i]);
  }
}
void getting_path()
{
  char *mypath = getenv("THEPATH");
  if(mypath == NULL)
    mypath = path_default;
  //printf("($THEPATH) is %s\n",mypath);
  number_of_paths = converting_array(mypath, ":", &actual_path);
}
void readline(char *input)
{
  scanf("%[^\n]", input);
  getchar();
}
int checking_implementing(char *filepath)
{
  struct stat buf;
  int rc = lstat(filepath, &buf );
  if(rc < 0)
    return 0;
  if ( S_ISREG( buf.st_mode ) )
  {
    if ( buf.st_mode & ( S_IXUSR | S_IXGRP | S_IXOTH ) )
    {
      return 1;
    }
  }

  return 0;
}
int finding_path(char *cmd, char *filepath)
{
  int i;
  char cur[MAX];
  for(i=0; i<number_of_paths; i++)
  {
    strcpy(cur,actual_path[i]);
    strcat(cur,"/");
    strcat(cur,cmd);
    if(checking_implementing(cur)){
      strcpy(filepath,cur);
      return 1;
    }
  }
  return 0;
}
int posting_array(char * str, char ** list, int size)
{
  int i;
  for(i = 0; i<size; i++)
  {
    if( strcmp(list[i],str)==0 )
      return i;
  }
  return -1;
}
void executing_path(int argslen, char * cmd, char ** args, char * filepath)
{
  switch(argslen){
    case 0:
      execlp(filepath, cmd, NULL); break;
    case 1:
      execlp(filepath, cmd, args[0], NULL); break;
    case 2:
      execlp(filepath, cmd, args[0], args[1], NULL); break;
    case 3:
      execlp(filepath, cmd, args[0], args[1], args[2], NULL); break;
    case 4:
      execlp(filepath, cmd, args[0], args[1], args[2], args[3], NULL); break;
  }
  perror( "execlp() failed!" );
  exit(EXIT_FAILURE);
}
void redirfd(int redirfd, char *pathname, int flags, mode_t mode)
{
  int fd;
  if((int)mode == -1){
    fd = open(pathname, flags);
  }
  else{
    fd = open(pathname, flags , mode);
  }
  if( fd < 0 )
  {
    perror("ERROR: Failed to operate file\n");
    exit(-1);
  }
  if((dup2(fd, redirfd) == -1))
  {
    perror("ERROR: Failed to redirect file descriptor\n");
    exit(-1);
  }
}
void child(int argslen, char * cmd, char ** args, char * filepath)
{
  int pos, cmdlen;
  cmdlen = argslen;
  if( (pos = posting_array("==>", args, argslen)) != -1 )
  {
    cmdlen = pos;
    redirfd(1, args[pos+1], O_WRONLY | O_CREAT | O_TRUNC , 0664);
  }
  else if( (pos = posting_array("<==", args, argslen)) != -1 )
  {
    cmdlen = pos;
    redirfd(0, args[pos+1], O_RDONLY, -1);
  }
  else if( (pos = posting_array("-->", args, argslen)) != -1 )
  {
    cmdlen = pos;
    redirfd(1, args[pos+1], O_WRONLY | O_CREAT | O_APPEND , 0664);
  }
  executing_path(cmdlen, cmd, args, filepath);
}
int verifyampersand(char ** inputlist, int size){
  int i;
  for(i = 0; i< size-1; i++){
    if(strcmp(inputlist[i], "&")==0){
      return -1;
    }
  }
  if(strcmp(inputlist[size-1], "&")==0)
    return 1;
  return 0;
}
void executing_command(char *cmd, char **args, int argslen, char *filepath)
{
  int ampersand = verifyampersand(args, argslen);
  if(ampersand == -1){
    printf("ERROR: & Must be at the end of command\n");
  }
  int pid = fork();
  if(pid < 0){
    perror( "fork() failed!" );
    exit(EXIT_FAILURE);
  }
  else if( pid == 0 ){
    if(ampersand == 1)
      child(argslen-1, cmd, args, filepath);
    else
      child(argslen, cmd, args, filepath);
  }
  else{
    if(ampersand != 1){
      int status;
      wait( &status );
    }
    else{
      printf("[process running in background with pid %d]\n",pid);
    }
  }
}
void handling_command(char *cmd, char **args, int argslen)
{
  char filepath[MAX];
  if(strcmp(cmd,"cd")==0){
    if(chdir(args[0])!=0){
      printf("cd: %s: No such file or directory\n", cmd);
    }
  }
  else if(finding_path(cmd, filepath)){
    executing_command(cmd, args, argslen, filepath);
  }
  else if( (strcmp(cmd, "quit")==0 || strcmp(cmd, "exit")==0) && argslen == 0){
    printf("bye\n");
    exit(EXIT_SUCCESS);
  }
  else{
    printf("ERROR: command '%s' not found.\n", cmd);
  }
}
