#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>
#include <regex.h>
#include <sys/poll.h>
#include <sys/select.h>



typedef struct {

    int pid;
    char* name;
    char state;
    float cpu_usage;
    float mem_usage;

} process;

typedef struct{

    float uptime;
    float clock;
    float page_size;
    float total_memory;

} sysinfo;




float compute_cpu_usage(long unsigned int stime, long unsigned int utime, long unsigned int starttime, sysinfo* sinfo) {

    float total_usage = (stime + utime)* 100;
    float elapsed_time = sinfo->uptime - (starttime/sinfo->clock);
    float cpu_usage = (total_usage/sinfo->clock)/ elapsed_time;
    return cpu_usage;
}

float compute_mem_usage(long unsigned int rss, sysinfo* sinfo){


    float mem_usage = ((rss * sinfo->page_size)/ sinfo->total_memory)*100;
 
    return mem_usage;
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



void setup_process(char* stats, process* proc, sysinfo* sinfo){
    
    char polished_stats[1000];

    long unsigned int stime_cpu;
    long unsigned int utime_cpu;
    long unsigned int starttime_cpu;
    long unsigned int rss;

    char* toks_stats = strtok(stats, "(");

    int tok_i = 0;
    while (toks_stats != NULL){

        if(tok_i == 0){

            proc->pid = atoi(toks_stats);
        }


        if(tok_i== 1) {

            int tok_sliced_i = 0;
            char* toks_sliced_stats = strtok(toks_stats, ")");

            while (toks_sliced_stats  != NULL){

                if(tok_sliced_i  == 0) {

                    proc->name = toks_sliced_stats;
                }

                if(tok_sliced_i  == 1) {
                    sprintf(polished_stats ,"%s", toks_sliced_stats);
                }

                tok_sliced_i++;            
                toks_sliced_stats = strtok(NULL, ")");
            }
        }

        tok_i++;            
        toks_stats = strtok(NULL, "(");

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
        } else if (stat_index == 21) {
            rss = atoi(tok);
        }
        stat_index++;            


        tok = strtok(NULL, " ");
    }

    proc->cpu_usage = compute_cpu_usage(stime_cpu, utime_cpu, starttime_cpu, sinfo);
    proc->mem_usage = compute_mem_usage(rss, sinfo);

    return;
}


void get_sysinfo(sysinfo* s){

    float total_memory ;
    char meminfo_buf[14];
    sprintf(meminfo_buf, "/proc/meminfo");
    FILE* meminfo_f = fopen(meminfo_buf, "r");
    fscanf(meminfo_f, "%*s %f", &total_memory); 
    fclose(meminfo_f);

    s->total_memory = total_memory;

    float uptime;
    char* buf = malloc(sizeof(char)*1000);
    sprintf(buf, "/proc/uptime");
    FILE* fd_uptime = fopen(buf, "r");
    fscanf(fd_uptime, "%f", &uptime);
    fclose(fd_uptime);

    s->uptime = uptime;    

    s->clock = sysconf(_SC_CLK_TCK);
    s->page_size = sysconf(_SC_PAGESIZE)/1000;

}


int process_monitor(){
    
    DIR *procDIR;
    struct dirent *processes_list;     
    procDIR = opendir ("/proc");

    sysinfo* sinfo = (sysinfo*) malloc(sizeof(sysinfo));
    get_sysinfo(sinfo);
    

    if(procDIR == NULL){
        perror("Couldn't open the directory");
        return -1;
    }

    process* procs[100];
    int count = 0;

    while (( processes_list = readdir (procDIR)) != NULL) {

        if(isdigit(processes_list->d_name[0])) {

            char stat_address_buf[512]; 
            sprintf(stat_address_buf, "/proc/%s/stat", processes_list->d_name);

            char stats_content_buf[512];
            readfile(stat_address_buf, stats_content_buf);

            process* proc = (process*) malloc(sizeof(process));  

            setup_process(stats_content_buf, proc, sinfo); 
            
            printf("%d   %c   %0.4f   %0.4f   %s\n", proc->pid, proc->state, proc->cpu_usage, proc->mem_usage, proc->name);  

            /*
            if(count < 100){
                procs[count] = proc;
                count++;
            }
            */
        }
    } 
    closedir(procDIR);

    /*
    for(int i = 0; i<count; i++){
       printf("%d   %c   %0.4f   %0.4f   %s\n", procs[i]->pid, procs[i]->state, procs[i]->cpu_usage, procs[i]->mem_usage, procs[i]->name);
    }
    */


    return 0;
}






int main(){

    while(1){
        system("clear");

        printf("       PID          STATE         CPU             MEM             COMMAND\n");

        process_monitor();

        sleep(1);
    }
}