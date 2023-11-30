#define _POSIX_C_SOURCE 200809L
#define AMOUNT 100
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int nb_operations, amount;

void *add(void *arg) {
    for (int i = 0; i < nb_operations; i++) {
        amount += AMOUNT;
        printf("Credit %lu : credit (%d) => solde = %d\n", pthread_self(), AMOUNT, amount);
    }
    pthread_exit(NULL);
}

void *sub(void *arg) {
    for (int i = 0; i < nb_operations; i++) {
        amount -= AMOUNT;
        printf("Debit %lu : debit (%d) => solde = %d\n", pthread_self(), AMOUNT, amount);
    }
    pthread_exit(NULL);
}

int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <Number of Threads> <Initial Amount> <Number of Operations>\n",
                argv[0]);
        exit(EXIT_FAILURE);
    }
    int nb_threads;
    if (sscanf(argv[1], "%d", &nb_threads) != 1 || sscanf(argv[2], "%d", &amount) != 1 ||
        sscanf(argv[3], "%d", &nb_operations) != 1) {
        perror("sscanf error");
        exit(EXIT_FAILURE);
    }
    pthread_t *threads = calloc(nb_threads, sizeof(pthread_t));
    int error_code;
    for (int i = 0; i < nb_threads; i++) {
        if (i % 2 == 0) {
            if ((error_code = pthread_create(&threads[i], NULL, add, NULL)) != 0) {
                fprintf(stderr, "pthread_create reuturned an error : %s", strerror(error_code));
                exit(EXIT_FAILURE);
            }
        } else {
            if ((error_code = pthread_create(&threads[i], NULL, sub, NULL)) != 0) {
                fprintf(stderr, "pthread_create reuturned an error : %s", strerror(error_code));
                exit(EXIT_FAILURE);
            }
        }
    }
    for (int i = 0; i < nb_threads; i++) {
        if ((error_code = pthread_join(threads[i], NULL)) != 0) {
            fprintf(stderr, "pthread_join reuturned an error : %s", strerror(error_code));
            exit(EXIT_FAILURE);
        }
    }
    free(threads);
    printf("Solde = %d\n", amount);
    exit(EXIT_SUCCESS);
}
