#define _POSIX_C_SOURCE 200809L
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void sighandler(int sig, siginfo_t *info, void *context);

int main(void) {
    pid_t my_pid = getpid();
    struct sigaction sa;
    sa.sa_flags = SIGINT;
    sa.sa_sigaction = sighandler;
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("Error : Sigaction setup failed");
        exit(EXIT_FAILURE);
    }
    while (true) {
        printf("Je suis le processus %d\n", my_pid);
        sleep(1);
    }
}

void sighandler(int sig, siginfo_t *info, void *context) {
    printf("%s recu par %d\n", strsignal(sig), info->si_pid);
}
