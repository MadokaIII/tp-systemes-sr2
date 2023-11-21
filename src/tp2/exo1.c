#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

void sighandler(int sig);

int communication[2], nb_messages, count = 0, intervals;

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Error : Not a valid number of arguments, I need 2.\n");
        exit(EXIT_FAILURE);
    }
    if (sscanf(argv[1], "%d", &nb_messages) != 1) {
        perror("Error : Failed scanning value");
        exit(EXIT_FAILURE);
    }
    if (sscanf(argv[2], "%d", &intervals) != 1) {
        perror("Error : Failed scanning value");
        exit(EXIT_FAILURE);
    }
    struct sigaction sa;
    sa.sa_flags = 0;
    sa.sa_handler = sighandler;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGALRM, &sa, NULL) == -1) {
        perror("Error : Sigaction setup for SIGALRM failed");
        exit(EXIT_FAILURE);
    }
    if (pipe(communication) == -1) {
        perror("Error : pipe failed");
        exit(EXIT_FAILURE);
    }
    switch (fork()) {
    case -1:
        perror("Error : fork failed");
        exit(EXIT_FAILURE);
    case 0:
        close(communication[1]);
        int i = 1;
        int was_read;
        char buffer[1];
        char current_time[80];
        time_t t;
        struct tm *tm;
        while ((was_read = read(communication[0], buffer, 1)) == 1) {
            t = time(NULL);
            tm = localtime(&t);
            strftime(current_time, sizeof(current_time), "%c", tm);
            printf("\tFils - Reçu de mon pere : Message numéro %d envoye a %s\n", i, current_time);
            i++;
        }
        if (was_read == -1) {
            perror("Error : read failed");
            exit(EXIT_FAILURE);
        }
        printf("\tFils - Je me termine\n");
        close(communication[0]);
        exit(EXIT_SUCCESS);
    default:
        close(communication[0]);
        alarm(intervals);
        while (wait(NULL) == -1) {
            if (errno == EINTR) {
                continue;
            } else {
                perror("Error : Wait call failed (no child or interrupted)");
                exit(EXIT_FAILURE);
            }
        }
        printf("\tPere - Je me termine en dernier\n");
        exit(EXIT_SUCCESS);
    }
}

void sighandler(int sig) {
    count++;
    char buffer[1];
    buffer[0] = 35;
    write(communication[1], buffer, 1);
    printf("Pere - Message numero %d envoye\n", count);
    if (count >= nb_messages) {
        close(communication[1]);
    } else {
        alarm(intervals);
    }
}
