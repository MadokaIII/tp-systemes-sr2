#include <sys/_types/_pid_t.h>
#define _POSIX_C_SOURCE 200809L
#define PRODUCTION 60
#define READ_RIGHTS(n) (n + 1) * 6
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "Error : Not a valid number of arguments, I need 3.\n");
        exit(EXIT_FAILURE);
    }
    int nb_sons, nb_production, nb_seconds;
    if (sscanf(argv[1], "%d", &nb_sons) != 1) {
        fprintf(stderr, "Error : Failed scanning 1st value.\n");
        exit(EXIT_FAILURE);
    }
    if (sscanf(argv[2], "%d", &nb_production) != 1) {
        fprintf(stderr, "Error : Failed scanning 2nd value.\n");
        exit(EXIT_FAILURE);
    }
    if (sscanf(argv[3], "%d", &nb_seconds) != 1) {
        fprintf(stderr, "Error : Failed scanning 3rd value.\n");
        exit(EXIT_FAILURE);
    }
    int communication[2];
    if (pipe(communication) == -1) {
        perror("Error : pipe failed");
        exit(EXIT_FAILURE);
    }
    char buffer[PRODUCTION];
    for (int i = 0; i < nb_sons; i++) {
        switch (fork()) {
        case -1:
            perror("Error : Fork failed.");
            exit(EXIT_FAILURE);
        case 0:
            close(communication[1]);
            int was_read, nb_boxes = 0;
            while ((was_read = read(communication[0], buffer, READ_RIGHTS(i))) == READ_RIGHTS(i)) {
                nb_boxes++;
                printf("Emballage %d : Nouvelle boite de %d produite\n", i, was_read);
            }
            if (was_read == -1) {
                perror("Error : read failed at some point");
                exit(EXIT_FAILURE);
            }
            printf("Emballage %d : J'ai produit %d boite de %d chocolats\n", i, nb_boxes,
                   READ_RIGHTS(i));
            close(communication[0]);
            exit(EXIT_SUCCESS);
        default:
            break;
        }
    }
    close(communication[0]);
    memset(buffer, 35, PRODUCTION);
    char current_time[80];
    time_t t;
    struct tm *tm;
    for (int i = 0; i < nb_production; i++) {
        t = time(NULL);
        tm = localtime(&t);
        strftime(current_time, sizeof(current_time), "%c", tm);
        printf("Usine : Je produis %d chocolats [%s]\n", PRODUCTION, current_time);
        write(communication[1], buffer, PRODUCTION);
        sleep(nb_seconds);
    }
    close(communication[1]);
    exit(EXIT_SUCCESS);
}
