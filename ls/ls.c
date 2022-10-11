#include <sys/types.h>
#include <string.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    //DIR *opendir(const char *filename);
    DIR *folder;
    
    //struct dirent *readdir(DIR *dirp);
    struct dirent *entry;
    

    if (argc > 2) 
    {
	fprintf(stderr, "usage: %s dir_name\n", argv[0]);
	exit(1);
    }
  
  
   if (argc == 1)
   {
  	folder = opendir(".");
   } 
   else
   {
	folder = opendir(argv[1]);
   }

    while( (entry=readdir(folder)) != NULL )
    {
    	if (entry->d_name[0] == '.')
    	{
    	continue;
    	}
        printf("%s\n",entry->d_name);
    }

    closedir(folder);

    return(0);
}
