#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void sighandler(int sig, siginfo_t *info, void *context);
void sensor_task(int sensor_id);

volatile sig_atomic_t sensor_1 = 0, sensor_2 = 0;
int max, threshold;

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Error : Invalid number of arguments, I need 3 of them\n");
        exit(EXIT_FAILURE);
    }
    if (sscanf(argv[1], "%d", &max) != 1) {
        perror("Error : Scanning the first argument failed");
        exit(EXIT_FAILURE);
    }
    if (sscanf(argv[2], "%d", &threshold) != 1) {
        perror("Error : Scanning the second argument failed");
        exit(EXIT_FAILURE);
    }
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = sighandler;
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("Error : Sigaction setup failed");
        exit(EXIT_FAILURE);
    }
    if (sigaction(SIGUSR2, &sa, NULL) == -1) {
        perror("Error : Sigaction setup failed");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < 2; ++i) {
        switch (fork()) {
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);
        case 0:
            sensor_task(i + 1);
            exit(EXIT_SUCCESS);
        default:
            break;
        }
    }
    int status;
    for (int i = 0; i < 2; i++) {
        while (wait(&status) == -1) {
            if (errno == EINTR) {
                continue;
            } else {
                perror("Error : Wait call failed (no child or interrupted)");
                exit(EXIT_FAILURE);
            }
        }
    }
    printf("Pere (%d) - Je me termine en dernier\n", getpid());
    exit(EXIT_SUCCESS);
}

void sighandler(int sig, siginfo_t *info, void *context) {
    if (sig == SIGUSR1) {
        sensor_1 += threshold;
        printf("Pere (%d) - Capteur 1 : Nombre de vehicules = %d\n", getpid(), sensor_1);
    } else {
        sensor_2 += threshold;
        printf("Pere (%d) - Capteur 2 : Nombre de vehicules = %d\n", getpid(), sensor_2);
    }
}

void sensor_task(int sensor_id) {
    char ch;
    int count = 0;
    while (count < max) {
        ch = getchar();
        if (ch == '\n') {
            continue;
        }
        count++;
        if (count % threshold == 0) {
            printf("\tCapteur %d (%d) : %d vehicules de plus => %d\n", sensor_id, getpid(),
                   threshold, count);
            kill(getppid(), (sensor_id == 1) ? SIGUSR1 : SIGUSR2);
        }
    }
    printf("\tCapteur %d (%d) : Termine\n", sensor_id, getpid());
}
