#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>

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

void quitting();
void get_sysinfo(sysinfo* s);
void readfile(const char* file_path,char* buf);

float compute_cpu_usage(long unsigned int stime, long unsigned int utime, long unsigned int starttime, sysinfo* sinfo);
float compute_mem_usage(long unsigned int rss, sysinfo* sinfo);
void sort_processes(process** procs);
void build_processes_buffer(process** procs, int count);
void setup_process(char* stats, process* proc, sysinfo* sinfo);
int process_monitor(sysinfo* sinfo, int mode);


void show_procs(sysinfo* sinfo, int mode);
void manage_procs(sysinfo* sinfo);