#define _POSIX_C_SOURCE 200809L
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

void sighandler(int sig);
void finish(int sig);

typedef struct s_tuple {
    int index;
    int value;
} Tuple;

Tuple *tuple;
volatile sig_atomic_t finished = 0;
int communication[2];
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
    if (pipe(communication) == -1) {
        perror("Error : pipe failed");
        exit(EXIT_FAILURE);
    }
    tuple = malloc(sizeof(Tuple));
    if (tuple == NULL) {
        perror("Error : malloc failed");
        exit(EXIT_FAILURE);
    }
    pid_t *sons_pid = calloc(nb_sons, sizeof(pid_t));
    for (int i = 0; i < nb_sons; i++) {
        switch (sons_pid[i] = fork()) {
        case -1:
            perror("Error : fork failed");
            exit(EXIT_FAILURE);
        case 0:
            free(sons_pid);
            close(communication[0]);
            struct sigaction sa;
            sa.sa_flags = 0;
            sa.sa_handler = sighandler;
            sigemptyset(&sa.sa_mask);
            if (sigaction(SIGALRM, &sa, NULL) == -1) {
                perror("Error : Sigaction setup for SIGALRM failed");
                exit(EXIT_FAILURE);
            }
            sa.sa_handler = finish;
            sigemptyset(&sa.sa_mask);
            if (sigaction(SIGINT, &sa, NULL) == -1) {
                perror("Error : Sigaction setup for SIGINT failed");
                exit(EXIT_FAILURE);
            }
            tuple->index = i;
            char ch;
            alarm(intervals);
            while (tuple->value < max) {
                ch = getchar();
                if (ch != '\n') {
                    tuple->value++;
                }
            }
            while (finished == 0)
                pause();
            free(tuple);
            printf("\tCapteur %d (%d) : Termine\n", i, getpid());
            close(communication[1]);
            exit(EXIT_SUCCESS);
        default:
            break;
        }
    }
    close(communication[1]);
    int was_read;
    while ((was_read = read(communication[0], tuple, sizeof(Tuple))) == sizeof(Tuple)) {
        printf("Pere (%d) - Capteur %d : Nombre de vehicules = %d\n", getpid(), tuple->index,
               tuple->value);
        if (tuple->value >= max) {
            printf("Pere (%d) - Signale au capteur %d (%d) sa fin\n", getpid(), tuple->index,
                   sons_pid[tuple->index]);
            kill(sons_pid[tuple->index], SIGINT);
        }
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
    free(sons_pid);
    free(tuple);
    close(communication[0]);
    printf("Pere (%d) - Je me termine en dernier\n", getpid());
    exit(EXIT_SUCCESS);
}

void sighandler(int sig) {
    char current_time[80];
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    strftime(current_time, sizeof(current_time), "%c", tm);
    printf("\tCapteur %d (%d) : Valeur %d ecrite dans tube a %s\n", tuple->index, getpid(),
           tuple->value, current_time);
    alarm(intervals);
}

void finish(int sig) { finished = 1; }