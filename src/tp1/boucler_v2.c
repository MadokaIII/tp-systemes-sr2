#define _POSIX_C_SOURCE 200809L
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
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
    pid_t pid = getpid();
    signal(SIGINT, SIG_IGN);
    printf("Processus de pid %d : Protege contre SIGINT\n", pid);
    printf("Processus de pid %d : Je vais devenir l'executable boucler pour afficher %d fois "
           "toutes les %d secondes\n",
           pid, nb_repeat, nb_seconds);
    execv("boucler", argv);
    perror("Error : execv failed");
    exit(EXIT_FAILURE);
}