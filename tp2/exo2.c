#include <signal.h>
#include <string.h>
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

void sighandler(int sig);

typedef struct s_tuple {
    int index;
    int value;
} Tuple;

int nb_sons, max, intervals;

int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "Error : Not a valid number of arguments, I need 3.\n");
        exit(EXIT_FAILURE);
    }
    if (sscanf(argv[1], "%d", &nb_sons) != 1) {
        perror("Error : Failed scanning value");
        exit(EXIT_FAILURE);
    }
    if (sscanf(argv[2], "%d", &max) != 1) {
        perror("Error : Failed scanning value");
        exit(EXIT_FAILURE);
    }
    if (sscanf(argv[3], "%d", &intervals) != 1) {
        perror("Error : Failed scanning value");
        exit(EXIT_FAILURE);
    }
    int communication[2];
    if (pipe(communication) == -1) {
        perror("Error : pipe failed");
        exit(EXIT_FAILURE);
    }
    Tuple *tuple = malloc(sizeof(Tuple));
    if (tuple == NULL) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < nb_sons; i++) {
        switch (fork()) {
        case -1:
            perror("Error : fork failed");
            exit(EXIT_FAILURE);
        case 0:
            close(communication[0]);
            struct sigaction sa;
            sa.sa_flags = 0;
            sa.sa_handler = sighandler;
            sigemptyset(&sa.sa_mask);
            if (sigaction(SIGALRM, &sa, NULL) == -1) {
                perror("Error : Sigaction setup for SIGALRM failed");
                exit(EXIT_FAILURE);
            }
            alarm(intervals);
            int count = 0;
            tuple->index = i;
            char ch;
            int n;
            while (count < max) {
                n = 0;
                ch = getchar();
                if (ch == '\n') {
                    continue;
                }
                count++;
                n++;
                if (count % threshold == 0) {
                    tuple->value = n;
                    printf("\tCapteur %d (%d) : %d vehicules de plus => %d\n", i, getpid(), n,
                           count);
                    if (write(communication[1], tuple, sizeof(Tuple)) != sizeof(Tuple)) {
                        perror("write failed");
                        exit(EXIT_FAILURE);
                    }
                }
            }
            free(tuple);
            printf("\tCapteur %d (%d) : Termine\n", i, getpid());
            close(communication[1]);
            exit(EXIT_SUCCESS);
        default:
            break;
        }
    }
    close(communication[1]);
    int *array = calloc(nb_sons, sizeof(int));
    int was_read;
    while ((was_read = read(communication[0], tuple, sizeof(Tuple))) == sizeof(Tuple)) {
        array[tuple->index] += tuple->value;
        printf("Pere (%d) - Capteur %d : Nombre de vehicules = %d\n", getpid(), tuple->index,
               array[tuple->index]);
    }
    if (was_read == -1) {
        perror("read failed");
        exit(EXIT_FAILURE);
    }
    close(communication[0]);
    for (int i = 0; i < nb_sons; i++) {
        if ((wait(NULL)) == -1) {
            perror("Error : Wait call failed (no child or interrupted)");
            exit(EXIT_FAILURE);
        }
    }
    free(tuple);
    free(array);
    printf("Pere (%d) - Je me termine en dernier\n", getpid());
    close(communication[0]);
    exit(EXIT_SUCCESS);
}
void sighandler(int sig) {}