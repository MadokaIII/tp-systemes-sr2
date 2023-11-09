#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Error : Not a valid number of arguments, I need 2.\n");
        exit(EXIT_FAILURE);
    }
    int nb_sons, nb_calls;
    if (sscanf(argv[1], "%d", &nb_sons) != 1) {
        fprintf(stderr, "Error : Failed scanning 1st value.\n");
        exit(EXIT_FAILURE);
    }
    if (sscanf(argv[2], "%d", &nb_calls) != 1) {
        fprintf(stderr, "Error : Failed scanning 2nd value.\n");
        exit(EXIT_FAILURE);
    }
    pid_t pid;
    int status;
    for (int i = 0; i < nb_sons; i++) {
        switch (fork()) {
        case -1:
            perror("Error : Fork failed.\n");
            exit(EXIT_FAILURE);
        case 0:
            pid = getpid();
            for (int j = 0; j < nb_calls; j++) {
                printf("Activite rang %d : Identifiant = %d\n", i, pid);
            }
            exit(EXIT_SUCCESS);
        default:
            break;
        }
    }
    for (int i = 0; i < nb_sons; i++) {
        if ((pid = wait(&status)) == -1) {
            perror("Error : Wait call failed (no child or interrupted)\n");
            exit(EXIT_FAILURE);
        }
        if (WIFEXITED(status)) {
            printf("Valeur retournee par le fils %d = %d\n", pid, WEXITSTATUS(status));
        } else {
            fprintf(
                stderr,
                "Valeur retournee par le fils %d = %d\n(Anomalie dans le code retour détecté)\n",
                pid, WEXITSTATUS(status));
        }
    }
    exit(EXIT_SUCCESS);
}
