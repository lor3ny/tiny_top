
#include "tt_lib.h"


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


void sort_processes(process** procs){

    //bubblesort
    return;
}


void build_processes_buffer(process** procs, int count){

    char* procs_buf = (char*) malloc(sizeof(char) * 128000);


    strcat(procs_buf, " _  _  _ .|_ _  _\n|||(_)| )||_(_)|");
    strcat(procs_buf,"\n\n--- PID ----- STATE ---------- CPU USAGE ------ MEM USAGE ------ COMMAND\n\n");

    for(int i = 0; i<count; i++){

        char p_buf[512];
        sprintf(p_buf,"     %d          %c             %0.8f        %0.8f       %s\n", procs[i]->pid, procs[i]->state, procs[i]->cpu_usage, procs[i]->mem_usage, procs[i]->name);
        strcat(procs_buf, p_buf);
    }

    strcat(procs_buf,"\n\n (q) quit\n (b) back\n (enter) update\n\n");

    printf("%s", procs_buf);

    free(procs_buf);
}


void setup_process(char* stats, process* proc, sysinfo* sinfo){
    
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
    proc->name = (char*) malloc(sizeof(char) * 64);
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

    proc->cpu_usage = compute_cpu_usage(stime_cpu, utime_cpu, starttime_cpu, sinfo);
    proc->mem_usage = compute_mem_usage(rss, sinfo);

    free(polished_stats);
    
    return;
}


int process_monitor(sysinfo* sinfo, int mode){
    
    DIR *procDIR;   
    struct dirent *processes_list;  
    get_sysinfo(sinfo);
    procDIR = opendir ("/proc");
    

    if(procDIR == NULL){
        perror("Couldn't open the directory");
        return -1;
    }

    process* procs[1000];
    int count = 0;
    
    while (( processes_list = readdir (procDIR)) != NULL) {

        if(isdigit(processes_list->d_name[0])) {

            char stat_address_buf[512]; 
            sprintf(stat_address_buf, "/proc/%s/stat", processes_list->d_name);

            char stats_content_buf[512];
            readfile(stat_address_buf, stats_content_buf);

            procs[count] = (process*) malloc(sizeof(process));  

            setup_process(stats_content_buf, procs[count], sinfo); 
            
            if(count < 20000){
                count++;
            }
            
        }
    } 

    sort_processes(procs);
    if(mode == 0){
        mode = sizeof(procs)/sizeof(process);
    }
    build_processes_buffer(procs, mode);

    rewinddir(procDIR);
    closedir(procDIR);

    return 0;

}