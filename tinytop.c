#include "tt_lib.h"

//gcc -o tinytop tinytop.c tt_process_monitor.c tt_utilities.c

void show_procs(int mode){

    char procs_buf[64000];

    while(1){ 
        
        system("clear");
        fflush(stdout);
        
        process_monitor(mode, procs_buf);

        int command;
        scanf("%d", &command);

        if(command == 1){
            manage_procs();
        } else if(command == 2) {
            break;
        }

    }

}

void manage_procs(){

        printf("  __    __ __ __\n |  |--|__|  |  .-----.----.\n |    <|  |  |  |  -__|   _|\n |__|__|__|__|__|_____|__|\n\n");
        printf(" (1) kill \n (2) terminate pid\n (3) suspend pid\n (4) resume pid\n ---\n (5) back\n\n");

        int pid;
        int action;

        
        printf(">> ");
        scanf("%d", &action);

        if (action == 5){
            return;
        }

        printf("PID: ");
        scanf("%d", &pid);
 
        if(action == 1){

            kill(pid, SIGKILL);

        } else if (action == 2){

            kill(pid, SIGTERM);

        } else if (action == 3) {

            kill(pid, SIGSTOP);

        } else if (action == 4) {

            kill(pid, SIGCONT);
        }
}
	

int main(){

    
    while(1){

        system("clear");
        fflush(stdout);

        printf("\n\n  w   w                  w\n w8ww w 8d8b. Yb  dP    w8ww .d8b. 88b.\n  8   8 8P Y8  YbdP      8   8' .8 8  8\n  Y8P 8 8   8   dP       Y8P `Y8P' 88P'\n               dP                  8\n\n\n");
        printf(" (1) show processes\n (2) show all processes \n (3) manage processes \n --- \n (4) quit\n\n");
        printf(">> ");

        int command;
        scanf("%d", &command);
        
        if(command == 1){

            show_procs(1);

        } else if (command == 2){

            show_procs(0);

        } else if (command == 3){

            manage_procs();

        } else if (command == 4) {

            quitting();
        }
        
    }


    exit(0);
}



