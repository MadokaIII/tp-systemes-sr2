#define _POSIX_C_SOURCE 200809L
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void sighandler(int sig, siginfo_t *info, void *context);

int main(void) {
    pid_t my_pid = getpid();
    struct sigaction sa;
    sa.sa_flags = SIGINT;
    sa.sa_sigaction = sighandler;
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("Error : Sigaction setup failed\n");
    }
    while (1) {
        printf("Je suis le processus %d\n", my_pid);
        sleep(1);
    }
}

void sighandler(int sig, siginfo_t *info, void *context) {
    printf("%s reçu par %d\n", strsignal(sig), info->si_pid);
    // Pour moi strsignal rends l'entier du signal dans un char *, je ne sais pas trop pourquoi.
}
