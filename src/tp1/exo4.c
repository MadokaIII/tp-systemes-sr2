#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

void sighandler(int sig, siginfo_t *info, void *context);
void sensor_task(int sensor_id);
void handle_sigint(int sig);
void print_counters(int sig);

volatile sig_atomic_t sensor_1 = 0, sensor_2 = 0, should_exit = 0;
int max, threshold, interval;

int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "Error : Invalid number of arguments, I need 4 of them\n");
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
    if (sscanf(argv[3], "%d", &interval) != 1) {
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
    struct sigaction print_act;
    print_act.sa_handler = print_counters;
    sigemptyset(&print_act.sa_mask);
    print_act.sa_flags = 0;
    if (sigaction(SIGALRM, &print_act, NULL) == -1) {
        perror("Error : Sigaction setup for SIGALRM failed");
        exit(EXIT_FAILURE);
    }
    struct sigaction int_act;
    int_act.sa_handler = handle_sigint;
    sigemptyset(&int_act.sa_mask);
    int_act.sa_flags = 0;
    if (sigaction(SIGINT, &int_act, NULL) == -1) {
        perror("Error : Sigaction setup for SIGINT failed");
        exit(EXIT_FAILURE);
    }
    alarm(interval);
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
    while (!should_exit)
        pause();
    print_counters(0);
    printf("Pere (%d) - Je me termine en dernier\n", getpid());
    exit(EXIT_SUCCESS);
}

void sighandler(int sig, siginfo_t *info, void *context) {
    if (sig == SIGUSR1) {
        sensor_1 += threshold;
    } else {
        sensor_2 += threshold;
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

void handle_sigint(int sig) { should_exit = 1; }

void print_counters(int sig) {
    char current_time[80];
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    strftime(current_time, sizeof(current_time), "%c", tm);
    printf("Pere (%d) - %s : Capteur 1 : Nombre de vehicules = %d\n", getpid(), current_time,
           sensor_1);
    printf("Pere (%d) - %s : Capteur 2 : Nombre de vehicules = %d\n", getpid(), current_time,
           sensor_2);
    if (sensor_1 >= max && sensor_2 >= max) {
        should_exit = 1;
    }
    if (!should_exit) {
        alarm(interval);
    }
}
