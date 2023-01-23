#include "tt_lib.h"

void ClearInputBuffer() 
{
	char c = 0;
	// Loop over input buffer and consume chars until buffer is empty
	while ((c = getchar()) != '\n' && c != EOF);
}


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


int digits_counter(long unsigned int digit){

    int n = digit;
    int digits_count = 0;

    do {
        n /= 10;
        ++digits_count;
    } while ( n != 0);

    return digits_count;

}