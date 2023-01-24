
#include "tt_lib.h"

#define MAX_PROCS 1000


float compute_cpu_usage(long unsigned int stime, long unsigned int utime, long unsigned int starttime) {

    float uptime;
    char buf[13];
    sprintf(buf, "/proc/uptime");
    FILE* fd_uptime = fopen(buf, "r");
    fscanf(fd_uptime, "%f", &uptime);
    fclose(fd_uptime);    

    float clock = sysconf(_SC_CLK_TCK);

    float total_usage = (stime + utime)* 100;
    float elapsed_time = uptime - (starttime/clock);
    float cpu_usage = (total_usage/clock)/ elapsed_time;

    return cpu_usage;
}

float compute_mem_usage(long unsigned int rss){

    float page_size = sysconf(_SC_PAGESIZE)/1000;

    float total_memory ;
    char meminfo_buf[14];
    sprintf(meminfo_buf, "/proc/meminfo");
    FILE* meminfo_f = fopen(meminfo_buf, "r");
    fscanf(meminfo_f, "%*s %f", &total_memory); 
    fclose(meminfo_f);

    float mem_usage = ((rss * page_size)/ total_memory)*100;

    return mem_usage;
}


void sort_processes(process** procs, long unsigned int procs_count){

    process* back_proc;

    int swapped;

    do {

        swapped = 0;
        for(int i = 0; i<procs_count-1; i++){

            if(procs[i]->cpu_usage < procs[i+1]->cpu_usage){

                swapped = 1;
                back_proc = procs[i];
                procs[i] = procs[i+1];
                procs[i+1] = back_proc;
            }

        }
        
    } while(swapped != 0);

    return;
}


void build_processes_buffer(process** procs, int count, char* procs_buf){

    strncpy(procs_buf,"",256000);

    strcat(procs_buf, "   ___  ___            _ _\n   |  \\/  |           (_) |\n   | .  . | ___  _ __  _| |_ ___  _ __\n   | |\\/| |/ _ \\| '_ \\| | __/ _ \\| '__|\n   | |  | | (_) | | | | | || (_) | |\n   \\_|  |_/\\___/|_| |_|_|\\__\\___/|_|");
    strcat(procs_buf,"\n\n---- PID ----- STATE ---------- CPU USAGE(%) ------- MEM USAGE(%) ------ COMMAND\n\n");

    for(int i = 0; i<count; i++){

        char p_buf[24];

        int digits_count = digits_counter(procs[i]->pid);

        sprintf(p_buf,"    %d", procs[i]->pid);

        for(int j = 0; j<(5-digits_count); j++){
            strcat(p_buf, " ");
        }

        char p2_buf[512];
        sprintf(p2_buf,"        %c              %0.8f        %0.8f       %s\n", procs[i]->state, procs[i]->cpu_usage, procs[i]->mem_usage, procs[i]->name);
        strcat(p_buf, p2_buf);
        strcat(procs_buf, p_buf);
    }

    strcat(procs_buf,"\n\n (0) update\n (1) manage procs\n ---\n (2) back\n\n>> ");

    printf("%s", procs_buf);
}


void setup_process(char* stats, process* proc){
    
    char* polished_stats = (char*) malloc(sizeof(char)*1024);

    long unsigned int stime_cpu;
    long unsigned int utime_cpu;
    long unsigned int starttime_cpu;
    long unsigned int rss;
    
    //Stats polishing, handling space character in command name in raw mode

    char buf_pid[16];
    char buf_name[64];
    int pid_i;
    int polish_i;
    int name_i;
    int offset;

    while(stats[pid_i] != '('){
        buf_pid[pid_i] = stats[pid_i];
        pid_i++;
    }
    buf_pid[pid_i] = '\0';
    
    name_i = pid_i+1;
    offset = name_i;
    while(stats[name_i] != ')'){
        buf_name[name_i-offset] = stats[name_i];
        name_i++;
    }
    buf_name[name_i-offset] = '\0';

    proc->pid = atoi(buf_pid);
    strcpy(proc->name, buf_name);

    polish_i = name_i+2;
    offset = polish_i;
    while(stats[polish_i] != '\0'){
        polished_stats[polish_i-offset] = stats[polish_i];
        polish_i++;
    }

    //Achiving parameters from the polished stats string

    int stat_index = 0;
    char* tok = strtok(polished_stats, " ");

    while (tok != NULL){

        if (stat_index == 0) 
            proc->state = *tok;

        else if (stat_index == 11) 
            stime_cpu = atoi(tok);

        else if (stat_index == 12)
            utime_cpu = atoi(tok);

        else if (stat_index == 19) 
            starttime_cpu = atoi(tok);

        else if (stat_index == 21) 
            rss = atoi(tok);

        stat_index++;            


        tok = strtok(NULL, " ");
    }

    //Compute cpu usage percentage and memory usage percentage

    proc->cpu_usage = compute_cpu_usage(stime_cpu, utime_cpu, starttime_cpu);
    proc->mem_usage = compute_mem_usage(rss);

    free(polished_stats);
    
    return;
}


int process_monitor(int mode, char* procs_buf){
    
    DIR *procDIR;   
    struct dirent *processes_list;  
    procDIR = opendir ("/proc");
    

    if(procDIR == NULL){
        perror("Couldn't open the directory");
        return -1;
    }

    process* procs[MAX_PROCS];
    int count = 0;

    for(int i = 0; i<MAX_PROCS; i++){
        procs[i] = (process*) malloc(sizeof(process));  
    }

    
    while (( processes_list = readdir (procDIR)) != NULL) {

        if(isdigit(processes_list->d_name[0])) {

            char stat_address_buf[512]; 
            sprintf(stat_address_buf, "/proc/%s/stat", processes_list->d_name);

            char stats_content_buf[512];
            readfile(stat_address_buf, stats_content_buf);

            setup_process(stats_content_buf, procs[count]); 
            
            count++;
            if(count >= MAX_PROCS){
                break;
            }
            
        }
    } 

    sort_processes(procs, count);

    int procs_count = count;
    if(mode == 1){
        procs_count = 20;
    }
    
    build_processes_buffer(procs, procs_count, procs_buf);

    rewinddir(procDIR);
    closedir(procDIR);

    for(int i = 0; i<1000; i++){
        free(procs[i]);
    }

    return 0;

}