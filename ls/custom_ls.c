#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <limits.h> 
#include <errno.h>
#include <string.h> 
#include <sys/stat.h>
#include <pwd.h>
#include <time.h>
#include <libgen.h>
#include <sys/types.h>
#include <grp.h>
#include "custom_ls.h"
//main function    
int main(int argc, char *argv[]) {
    //initialize flags
    int a_flag = 0; 
    int b_flag = 0; 
    int c_flag = 0; 

    //get options 
    int opt; 
    while((opt = getopt(argc, argv, "al")) != -1) {
        switch (opt) {
        case 'l':   
            a_flag = 1; 
            break;
        case 'a':
            b_flag = 1; 
            break;
        default:
            fprintf(stderr, "ls: supports -l and -a options\n"); 
            exit(EXIT_FAILURE); 
        }
    }

    //check command line args and call print_args with appropriate parameters 
    if(optind == argc) {
        print_args(".", "NULL", b_flag, a_flag, c_flag);
        if(a_flag == 0) {
            printf("\n");
        }
    }else{
        while(optind < argc) {
            struct stat argbuf;
            char *arg = argv[optind]; 
            if((stat(arg, &argbuf)) == -1) {
                printf("ls: cannot access '%s': No such file or directory\n", argv[optind]);
            }else{
                if(S_ISREG(argbuf.st_mode)) {
                    c_flag = 1;
                    print_args(".", arg, b_flag, a_flag, c_flag); 
                }
                if(S_ISDIR(argbuf.st_mode)) {
                    printf("%s:\n", arg); 
                    print_args(arg, "NULL", b_flag, a_flag, c_flag); 
                }
                c_flag = 0;
                if(optind < argc-1) {
                    printf("\n"); 
                }
                if(a_flag == 0) {
                    printf("\n");
                }
            }
            optind ++; 
        }    
    }
}

//function to print file/directory data with ls option -l
void for_option_l_print(char *arguments_of_directory, struct dirent *entry_for_directory) {
    struct stat buffer_of_stats; 
    char file_path[PATH_MAX];
    sprintf(file_path, "%s/%s", arguments_of_directory, entry_for_directory->d_name);
    if(stat(file_path, &buffer_of_stats) == -1) {
        perror("stat");
        return;   
    }

    //permission data/nlink 
    printf((S_ISDIR(buffer_of_stats.st_mode)) ? "d" : "-"); 
    printf((buffer_of_stats.st_mode & S_IRUSR) ? "r" : "-");
    printf((buffer_of_stats.st_mode & S_IWUSR) ? "w" : "-");
    printf((buffer_of_stats.st_mode & S_IXUSR) ? "x" : "-");
    printf((buffer_of_stats.st_mode & S_IRGRP) ? "r" : "-");
    printf((buffer_of_stats.st_mode & S_IWGRP) ? "w" : "-");
    printf((buffer_of_stats.st_mode & S_IXGRP) ? "x" : "-");
    printf((buffer_of_stats.st_mode & S_IROTH) ? "r" : "-");
    printf((buffer_of_stats.st_mode & S_IWOTH) ? "w" : "-");
    printf((buffer_of_stats.st_mode & S_IXOTH) ? "x " : "- ");
    printf("%li ", buffer_of_stats.st_nlink);

    //group and user data 
    struct passwd *my_passwd; 
    struct group *my_groupid; 
    my_passwd = getpwuid(buffer_of_stats.st_uid);  
    if(my_passwd == NULL) {
        perror("getpwuid"); 
        printf("%d ", buffer_of_stats.st_uid); 
    }else {
        printf("%s ", my_passwd->pw_name); 
    }
    my_groupid = getgrgid(buffer_of_stats.st_gid);
    if(my_groupid == NULL) {
        perror("getpwuid"); 
        printf("%d ", buffer_of_stats.st_gid); 
    }else  {
        printf("%s ", my_groupid->gr_name); 
    }

    //file size
    printf("%5ld ", buffer_of_stats.st_size);

    //timestamp
    struct tm *time_stamp;
    char outstr[200];
    time_t t = buffer_of_stats.st_mtime;
    time_stamp = localtime(&t);   
    if(time_stamp == NULL) {
        perror("localtime"); 
        exit(EXIT_FAILURE);
    } 
    strftime(outstr, sizeof(outstr), "%b %d %R", time_stamp); 
    printf("%s ", outstr);

    //file name 
    printf("%s\n", entry_for_directory->d_name); 
}

//function to check flags and print file/directory info accordingly
void handling_flag(char *arguments_of_directory, struct dirent *entry_for_directory, int flag_all, int long_flag) {
    if(flag_all == 0){
        if((entry_for_directory->d_name[0] == '.')) { 
            return; 
        } 
    }
    if(long_flag == 0) {
        printf("%s ", entry_for_directory->d_name);
    }else { 
        for_option_l_print(arguments_of_directory, entry_for_directory);
    }
}

//function to handle cmd-line args
void print_args(char *arguments_of_directory, char *file, int all_flags, int long_flags, int file_flags) {
    //open directory
    DIR *dir = opendir(arguments_of_directory);
    if(dir == NULL) {
        perror("opendir"); 
        exit(EXIT_FAILURE);
    } 

    //read and print directory/file data 
    struct dirent *dir_entry;  
    errno = 0; 
    while((dir_entry = readdir(dir))!= NULL) { 
        if(file_flags == 1) {
            if(strcmp(dir_entry->d_name, file) == 0) {
                handling_flag(arguments_of_directory, dir_entry, all_flags, long_flags);
            }
        }else {
            handling_flag(arguments_of_directory, dir_entry, all_flags, long_flags); 
        }
    }
    if((dir_entry == NULL) && (errno != 0)) {
        perror("readdir");
        exit(EXIT_FAILURE); 
    }
    
    //close directory
    closedir(dir);
}