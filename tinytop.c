#include "tt_lib.h"

//gcc -o tinytop tinytop.c tt_process_monitor.c tt_utilities.c

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
        printf(" (k) kill \n (t) terminate pid\n (s) suspend pid\n (r) resume pid\n (b) back\n\n");

        int pid;
        char action;

        /*
        printf("Action: ");
        scanf("%c", &action);
        printf("%c\n",action);
        printf("PID: ");
        scanf("%d", &pid);
        */

        /*
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
        */
        

}
	

int main(){

    system("clear");

    sysinfo* sinfo = (sysinfo*) malloc(sizeof(sysinfo));
    
    while(1){

        system("clear");

        printf("\n\n  w   w                  w\n w8ww w 8d8b. Yb  dP    w8ww .d8b. 88b.\n  8   8 8P Y8  YbdP      8   8' .8 8  8\n  Y8P 8 8   8   dP       Y8P `Y8P' 88P'\n               dP                  8\n\n\n");
        printf(" (s) show processes\n (a) show all processes :(\n (m) manage processes \n (q) quit\n\n");

        char command;
        command = getchar();
        
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
    
    free(sinfo);

    exit(0);
}



