#include "tt_lib.h"


void quitting(){
    system("clear");
    printf("\n\n\n   _______ ___ _______ _______    __ ___\n  |   _   |   |   _   |   _   |  |__|_  `.\n  |.  1___|.  |.  1   |.  |   |   __  |  |\n  |.  |___|.  |.  _   |.  |   |  |__|_|  |\n  |:  1   |:  |:  |   |:  1   |     |___,'\n  |::.. . |::.|::.|:. |::.. . |\n  `-------`---`--- ---`-------'\n\n\n");
    exit(0);
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