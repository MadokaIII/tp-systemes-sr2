#define _POSIX_C_SOURCE 200809L
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void sighandler(int sig, siginfo_t *info, void *context);

int main(void) {
    pid_t son_pid;
    switch (fork()) {
    case -1:
        perror("Error : Fork failed");
        exit(EXIT_FAILURE);
    case 0:
        son_pid = getpid();
        struct sigaction sa;
        sa.sa_flags = SIGINT;
        sa.sa_sigaction = sighandler;
        if (sigaction(SIGINT, &sa, NULL) == -1) {
            perror("Error : Sigaction setup failed");
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < 10; i++) {
            printf("Je suis le processus %d\n", son_pid);
            sleep(1);
        }
        exit(EXIT_SUCCESS);
    default:
        break;
    }
    signal(SIGINT, SIG_IGN);
    int status;
    if ((son_pid = wait(&status)) == -1) {
        perror("Error : Wait call failed (no child or interrupted)");
        exit(EXIT_FAILURE);
    }
    if (WIFEXITED(status)) {
        printf("[Father] My son %d finished with %d as his exit code\n", son_pid,
               WEXITSTATUS(status));
    } else {
        fprintf(stderr, "[Father] My son %d finished due to an error with %d as his exit code\n",
                son_pid, WEXITSTATUS(status));
    }
    exit(EXIT_SUCCESS);
}

void sighandler(int sig, siginfo_t *info, void *context) {
    printf("%s reçu par %d\n", strsignal(sig), info->si_pid);
}
