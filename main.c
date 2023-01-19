#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>
#include <regex.h>



typedef struct {

    int pid;
    char* name;
    float stime_cpu;
    float utime_cpu;
    float starttime_cpu;

    float cpu_usage;
    float mem_usage;

} process;



float uptime_finder(){
    
    float uptime;
    char* buf = malloc(sizeof(char)*1000);
    sprintf(buf, "/proc/%s", "uptime");
    FILE* fd_uptime = fopen(buf, "r");
    fscanf(fd_uptime, "%f", &uptime);
    fclose(fd_uptime);

    return uptime;
}


float compute_cpu_usage(float stime, float ultime, float uptime, float starttime, float clock){

    float total_usage = (stime + ultime)* 100;
    float elapsed_time = uptime - (starttime/clock);

    float cpu_usage = (total_usage/clock)/elapsed_time;

}

float compute_mem_usage(){
    return 0.0;
}


void readfile(const char* file_path,char* buf){

    FILE *fd = fopen(file_path, "r");

    char c;

    int str_index = 0;
    do{
        c = fgetc(fd);
        
        buf[str_index] = c;
        str_index++;

    } while (c != EOF);

    fclose(fd);
}

int count_spaces(char* str){
    int index_name = 0;
    char* tok_name = strtok(str, " ");


    while (tok_name != NULL){

        index_name++;            
        tok_name = strtok(NULL, " ");
    }

    return index_name;
}

void setup_process(char* stats){

    char buf[100];
    int index_name = 0;
    char* tok_name = strtok(stats, "(");


    while (tok_name != NULL){

        if(index_name == 1)
            sprintf(buf, "%s" ,tok_name);

        index_name++;            
        tok_name = strtok(NULL, "(");
    }
    printf("%s\n", buf);
    

    /*


    int stat_index = 0;
    char* tok = strtok(stats, " ");

    printf("------------- new process ------------- \n");

    process* proc = (process*) malloc(sizeof(process));

    while (tok != NULL){






        if (stat_index == 0) {
            printf("PID: %s\n", tok);
        } else if (stat_index == 1) { 

            int count = count_spaces(tok);

            printf("NAME: %s\n", tok);
            printf("NAME COUNT: %d\n", count);



        } else if (stat_index == 2) { 
            printf("state: %s\n", tok);
        } else if (stat_index == 13) {
            proc->stime_cpu = atoi(tok);
            printf("stime: %s\n", tok);
        } else if (stat_index == 14) {
            proc->utime_cpu = atoi(tok);
            printf("utime: %s\n", tok);
        } else if (stat_index == 21) {
            proc->starttime_cpu = atoi(tok);
            printf("starttime: %s\n", tok);
        }
        stat_index++;            


        tok = strtok(NULL, " ");
    }
    */
    

}


int process_monitor(){
    
    DIR *dp;
    struct dirent *ep;     
    dp = opendir ("/proc");

    float uptime = uptime_finder();

    
    if (dp != NULL) {
        while ((ep = readdir (dp)) != NULL) {

            if(!isdigit(ep->d_name[0]))
                continue;

            char* stat_proc = NULL;
            stat_proc = malloc(sizeof(char)*255);
            strcat(stat_proc, "/proc/");
            strcat(stat_proc, ep->d_name);
            strcat(stat_proc,"/stat");

            char buf[1000];
            readfile(stat_proc, buf);
            setup_process(buf);


            /*
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

            int page_size = sysconf(_SC_PAGESIZE)/1000;
            float clock = sysconf(_SC_CLK_TCK);


            while(stat_value != NULL) {


                if (stat_index == 0)
                    printf("PID: %s\n", stat_value);
                else if (stat_index == 1)
                    printf("PID: %s\n", stat_value);
                else if (stat_index == 14)  
                    stime_cpu_value = atoi(stat_value);
                else if (stat_index == 15)
                    ultime_cpu_value = atoi(stat_value);
                else if (stat_index == 22)
                    starttime_cpu_value = atoi(stat_value);

                stat_index++;            
                stat_value = strtok(NULL, " ");
            }


            float cpu_usage = compute_cpu_usage(stime_cpu_value, ultime_cpu_value, uptime, starttime_cpu_value, clock);

            float mem_usage;


            printf("CPU USAGE: %.20f\n", cpu_usage);


            stat_proc = NULL;

            */
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