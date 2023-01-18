#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>

int main(){
    DIR *dp;
    struct dirent *ep;     
    dp = opendir ("/proc");
    if (dp != NULL) {
    while ((ep = readdir (dp)) != NULL)
        puts (ep->d_name);
          
        (void) closedir (dp);
        return 0;
    } else {
        perror ("Couldn't open the directory");
        return -1;
    }
}