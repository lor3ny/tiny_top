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
    char state;
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


float compute_cpu_usage(float stime, float utime, float starttime) {

    float uptime = uptime_finder();
    float clock = sysconf(_SC_CLK_TCK);

    float total_usage = (stime + utime)* 100;
    float elapsed_time = uptime - (starttime/clock);
    float cpu_usage = (total_usage/clock)/ elapsed_time;
    return cpu_usage;
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


void setup_process(char* stats){

    printf("------------- new process ------------- \n");

    process* proc = (process*) malloc(sizeof(process));

    int index_name = 0;
    char* tok_name = strtok(stats, "(");
    char polished_stats[1000];

    long unsigned int stime_cpu;
    long unsigned int utime_cpu;
    long unsigned int starttime_cpu;


    while (tok_name != NULL){

        if(index_name == 0){
            printf("%s\n", tok_name);
            proc->pid = atoi(tok_name);
        }


        if(index_name == 1) {
            int index_name2 = 0;
            char* tok_name2 = strtok(tok_name, ")");

            while (tok_name2 != NULL){

                if(index_name2 == 0) {
                    printf("%s\n", tok_name2);
                    proc->name = tok_name2;
                }

                if(index_name2 == 1) {
                    sprintf(polished_stats ,"%s", tok_name2);
                }

                index_name2++;            
                tok_name2 = strtok(NULL, ")");
            }
        }

        index_name++;            
        tok_name = strtok(NULL, "(");

    }


    
    int stat_index = 0;
    char* tok = strtok(polished_stats, " ");


    while (tok != NULL){


        if (stat_index == 0) {
            proc->state = *tok;
        } else if (stat_index == 11) {
            stime_cpu = atoi(tok);
        } else if (stat_index == 12) {
            utime_cpu = atoi(tok);
        } else if (stat_index == 19) {
            starttime_cpu = atoi(tok);
        }
        stat_index++;            


        tok = strtok(NULL, " ");
    }

    proc->cpu_usage = compute_cpu_usage(stime_cpu, utime_cpu,starttime_cpu);

    proc->mem_usage = compute_mem_usage();


    printf("%0.4f\n", proc->cpu_usage);

}


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