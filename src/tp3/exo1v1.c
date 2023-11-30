#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <Number of printer> <Number of prints>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int nb_printer, nb_print;
    if (sscanf(argv[1], "%d", &nb_printer) != 1 || sscanf(argv[2], "%d", &nb_print) != 1) {
        perror("sscanf error");
        exit(EXIT_FAILURE);
    }
    pid_t *pids = calloc(nb_printer, sizeof(pid_t));
    for (int i = 0; i < nb_printer; i++) {
        switch (pids[i] = fork()) {
        case -1:
            perror("fork error");
            exit(EXIT_FAILURE);
        case 0:
            for (int j = 0; j < nb_print; j++) {
                printf("Activite rang %d : identifiant = %d\n", i, getpid());
            }
            exit(EXIT_SUCCESS);
        default:
            break;
        }
    }
    for (int i = 0; i < nb_printer; i++) {
        wait(NULL);
        printf("Valeur retourne par le fils %d = %d\n", pids[i], i);
    }
    free(pids);
    return 0;
}