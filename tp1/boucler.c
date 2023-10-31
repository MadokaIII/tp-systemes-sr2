#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Error : Invalid number of arguments, I need 2 of them\n");
        exit(EXIT_FAILURE);
    }
    int nb_repeat, interval;
    if (sscanf(argv[1], "%d", &nb_repeat) != 1) {
        perror("Error : Scanning the first argument failed\n");
        exit(EXIT_FAILURE);
    }
    if (sscanf(argv[2], "%d", &interval) != 1) {
        perror("Error : Scanning the second argument failed\n");
        exit(EXIT_FAILURE);
    }

}
