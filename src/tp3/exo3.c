#define _POSIX_C_SOURCE 200809L
#define MAX_SIZE 30
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int nb_deposit, nb_withdrawal, size;
char **args;

void *productor(void *arg) {
    int *index = (int *)arg;
    char str[MAX_SIZE];
    int j = 0;
    for (int i = 0; i < nb_deposit; i++) {
        snprintf(str, MAX_SIZE, "Bonjour %d de prod %d", i, *index);
        printf("Prod %d (%lu) : Message depose = %s\n", *index, pthread_self(), str);
        args[j] = str;
        j++;
        if (j == size) {
            j = 0;
        }
    }
    pthread_exit(NULL);
}

void *consumer(void *arg) {
    int *index = (int *)arg;
    int j = 0;
    for (int i = 0; i < nb_withdrawal; i++) {
        printf("\tConso %d (%lu) : Message lu = %s\n", *index, pthread_self(), args[j]);
        j++;
        if (j == size) {
            j = 0;
        }
    }
    pthread_exit(NULL);
}

int main(int argc, char **argv) {
    if (argc != 6) {
        fprintf(stderr,
                "Usage: %s <Number of Productors> <Number of Consumers> <Number of Deposit> "
                "<Number of Withdrawals> <Size of Array>\n",
                argv[0]);
        exit(EXIT_FAILURE);
    }
    int nb_productors, nb_consumers;
    if (sscanf(argv[1], "%d", &nb_productors) != 1 || sscanf(argv[2], "%d", &nb_consumers) != 1 ||
        sscanf(argv[3], "%d", &nb_deposit) != 1 || sscanf(argv[4], "%d", &nb_withdrawal) != 1 ||
        sscanf(argv[5], "%d", &size) != 1) {
        perror("sscanf error");
        exit(EXIT_FAILURE);
    }
    args = calloc(size, sizeof(char) * MAX_SIZE);
    memset(args, 0, size * sizeof(char) * MAX_SIZE);
    int error_code;
    pthread_t *threads = calloc(nb_productors + nb_consumers, sizeof(pthread_t));
    int *ranks = calloc(nb_productors + nb_consumers, sizeof(int));
    for (int i = 0; i < nb_productors; i++) {
        ranks[i] = i;
        if ((error_code = pthread_create(&threads[i], NULL, productor, &ranks[i])) != 0) {
            fprintf(stderr, "pthread_create error : %s\n", strerror(error_code));
            exit(EXIT_FAILURE);
        }
    }
    for (int i = nb_productors; i < nb_productors + nb_consumers; i++) {
        ranks[i] = i;
        if ((error_code = pthread_create(&threads[i], NULL, consumer, &ranks[i]) != 0)) {
            fprintf(stderr, "pthread_create error : %s\n", strerror(error_code));
            exit(EXIT_FAILURE);
        }
    }
    for (int i = 0; i < nb_productors + nb_consumers; i++) {
        if ((error_code = pthread_join(threads[i], NULL)) != 0) {
            fprintf(stderr, "pthread_join error : %s\n", strerror(error_code));
            exit(EXIT_FAILURE);
        }
    }
    free(args);
    free(ranks);
    free(threads);
    exit(EXIT_SUCCESS);
}