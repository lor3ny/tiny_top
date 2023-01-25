#include "tt_lib.h"

//gcc -o tinytop tinytop.c tt_process_monitor.c tt_utilities.c	

int main(){

    int res;

    while(1){
        system("clear");
        fflush(stdout);

        printf("\n\n  w   w                  w\n w8ww w 8d8b. Yb  dP    w8ww .d8b. 88b.\n  8   8 8P Y8  YbdP      8   8' .8 8  8\n  Y8P 8 8   8   dP       Y8P `Y8P' 88P'\n               dP                  8\n\n\n");
        printf(" (1) show processes\n (2) show all processes \n (3) manage processes \n --- \n (4) quit\n\n");

        int command;
        while(1) {
            printf(">> ");

            res = fscanf(stdin,"%d", &command);
            ClearInputBuffer();
            if(res == 0 || command < 1 || command > 4){
                printf("Not permitted input\n");
                continue;
            }
            break;
        }

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



