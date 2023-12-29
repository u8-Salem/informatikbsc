#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>


int main(int argc, char ** argv) {

    printf("Hallo Welt\n");
    printf("user: %d\n", getuid());
    printf("group: %d\n", getgid());
    
}

