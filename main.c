#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>



int process_monitor(){
DIR *dp;
    struct dirent *ep;     
    dp = opendir ("/proc");
    
    if (dp != NULL) {
        while ((ep = readdir (dp)) != NULL) {

            if(!isdigit(ep->d_name[0]))
                continue;


            char* stat_proc = NULL;
            stat_proc = malloc(sizeof(char)*255);
            strcat(stat_proc, "/proc/");
            strcat(stat_proc, ep->d_name);
            strcat(stat_proc,"/stat");

            FILE *stat_ptr = fopen(stat_proc, "r");
            
            char* stat_str = NULL;
            stat_str = malloc(sizeof(char)*255);
            fgets(stat_str, 100, stat_ptr);
            printf("%s\n", stat_str);

            stat_proc = NULL;

            
        }             
        (void) closedir (dp);
        return 0;

    } else {
        perror ("Couldn't open the directory");
        return -1;
    }
}

int main(){
    process_monitor();
}