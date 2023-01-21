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
#include <pthread.h>
#include <sys/poll.h>
#include <signal.h>
#include <termios.h>



int quit = 0;

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

void quitting(){
    system("clear");
    printf("\n\n\n   _______ ___ _______ _______    __ ___\n  |   _   |   |   _   |   _   |  |__|_  `.\n  |.  1___|.  |.  1   |.  |   |   __  |  |\n  |.  |___|.  |.  _   |.  |   |  |__|_|  |\n  |:  1   |:  |:  |   |:  1   |     |___,'\n  |::.. . |::.|::.|:. |::.. . |\n  `-------`---`--- ---`-------'\n\n\n");
    exit(0);
}


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

void sort_processes(process** procs){


    //bubblesort
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
}

void show_procs(sysinfo* sinfo, int mode){

    while(1){
        
        system("clear");
        process_monitor(sinfo, mode);

        char c;
        c = getchar();

        if(c == 'q'){
            quitting();
        } else if(c == 'b') {
            break;
        }

    }


}

void manage_procs(sysinfo* sinfo){

        system("clear");
        printf("  __    __ __ __\n |  |--|__|  |  .-----.----.\n |    <|  |  |  |  -__|   _|\n |__|__|__|__|__|_____|__|\n\n");
        printf(" - kill pid\n - terminate pid\n - suspend pid\n - resume pid\n - back\n\n");

        int pid;
        char action;

        printf("Action: ");
        scanf("%c", &action);
        printf("%c\n",action);
        printf("PID: ");
        scanf("%d", &pid);

        if(action == 'k'){

            kill(pid, SIGKILL);

        } else if (action == 't'){

            kill(pid, SIGTERM);

        } else if (action == 's') {

            kill(pid, SIGSTOP);

        } else if (action == 'r') {

            kill(pid, SIGCONT);

        } else if (action == 'q'){
            quitting();
        }
        

}

void start_menu(sysinfo* sinfo){

    while(1){

        system("clear");

        printf("\n\n  w   w                  w\n w8ww w 8d8b. Yb  dP    w8ww .d8b. 88b.\n  8   8 8P Y8  YbdP      8   8' .8 8  8\n  Y8P 8 8   8   dP       Y8P `Y8P' 88P'\n               dP                  8\n\n\n");
        printf(" (s) show processes\n (a) show all processes :(\n (m) manage processes \n (q) quit\n\n");

        char command;
        scanf("%c", &command);
        printf("%c\n",command);

        
        if(command == 's'){
            show_procs(sinfo, 20);
        } else if (command == 'a'){
            show_procs(sinfo, 0);
        } else if (command == 'm'){
            manage_procs(sinfo);
        } else if (command == 'q') {
            quitting();
        }
        

    }

}
	

int main(){

    system("clear");

    sysinfo* sinfo = (sysinfo*) malloc(sizeof(sysinfo));
    
    start_menu(sinfo);
    
    free(sinfo);
}



