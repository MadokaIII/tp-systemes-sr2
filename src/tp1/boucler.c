#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Error : Invalid number of arguments, I need 2 of them\n");
        exit(EXIT_FAILURE);
    }
    int nb_repeat, nb_seconds;
    if (sscanf(argv[1], "%d", &nb_repeat) != 1) {
        perror("Error : Scanning the first argument failed");
        exit(EXIT_FAILURE);
    }
    if (sscanf(argv[2], "%d", &nb_seconds) != 1) {
        perror("Error : Scanning the second argument failed");
        exit(EXIT_FAILURE);
    }
    char current_time[80];
    time_t t;
    struct tm *tm;
    pid_t pid = getpid();
    for (int i = 0; i < nb_repeat; i++) {
        t = time(NULL);
        tm = localtime(&t);
        strftime(current_time, sizeof(current_time), "%c", tm);
        printf("Je suis le processus %d, il est %s\n", pid, current_time);
        sleep(nb_seconds);
    }
    exit(EXIT_SUCCESS);
}
