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

            char* stat_value; 
            int stat_index = 0;

            stat_value = strtok(stat_str, " ");

            float stime_cpu_value;
            float ultime_cpu_value;
            float starttime_cpu_value;

            while(stat_value != NULL) {


                if (stat_index == 0)
                    printf("PID: %s\n", stat_value);
                else if (stat_index == 1)
                    printf("PID: %s\n", stat_value);
                else if (stat_index == 13)  
                    stime_cpu_value = atoi(stat_value);
                else if (stat_index == 14)
                    ultime_cpu_value = atoi(stat_value);
                else if (stat_index == 22)
                    starttime_cpu_value = atoi(stat_value);

                stat_index++;            
                stat_value = strtok(NULL, " ");
            }

            float cpu_usage = (stime_cpu_value + ultime_cpu_value)/starttime_cpu_value;
            printf("CPU USAGE: %.6f\n", cpu_usage);


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