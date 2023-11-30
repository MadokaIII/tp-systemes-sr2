#define _POSIX_C_SOURCE 200809L
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int nb_print;

void *print_activity(void *arg) {
    int *rank = (int *)arg;
    for (int i = 0; i < nb_print; i++) {
        printf("Activite rang %d : identifiant = %ld\n", *rank, pthread_self());
    }
    pthread_exit(rank);
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <Number of printer> <Number of prints>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int nb_printer;
    if (sscanf(argv[1], "%d", &nb_printer) != 1 || sscanf(argv[2], "%d", &nb_print) != 1) {
        perror("sscanf error");
        exit(EXIT_FAILURE);
    }
    pthread_t *threads = calloc(nb_printer, sizeof(pthread_t));
    int *ranks = calloc(nb_printer, sizeof(int));
    int error_code;
    for (int i = 0; i < nb_printer; i++) {
        ranks[i] = i;
        if ((error_code = pthread_create(&threads[i], NULL, print_activity, &ranks[i])) != 0) {
            fprintf(stderr, "pthread_create reutrned : %s", strerror(error_code));
            exit(EXIT_FAILURE);
        }
    }
    int *res;
    for (int i = 0; i < nb_printer; i++) {
        if ((error_code = pthread_join(threads[i], (void **)&res)) != 0) {
            fprintf(stderr, "pthread_join reutrned : %s", strerror(error_code));
            exit(EXIT_FAILURE);
        }
        printf("Valeur retourne par le fils %lu = %d\n", threads[i], *res);
    }
    free(ranks);
    free(threads);
    return 0;
}