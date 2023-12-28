#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char ** argv) 
{
    printf("Hallo Welt!\n");
    printf("user: %d\n", getuid());
    printf("group: %d\n", getgid());
    return 0;
}

// PUNKTE: 1