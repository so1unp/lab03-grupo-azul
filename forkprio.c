#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <string.h>

static struct timespec child_start;

void handler(int sig) {
    if (sig == SIGTERM) {
        struct rusage usage;
        getrusage(RUSAGE_SELF,&usage);
        long seconds = usage.ru_utime.tv_sec + usage.ru_stime.tv_sec;
        printf("Child %d (nice %2d):\t%3ld\n", 
               getpid(), 
               getpriority(PRIO_PROCESS, 0), 
               seconds);
        fflush(stdout);
        exit(EXIT_SUCCESS);
    }
}

int busywork(void) {
    struct tms buf;
    for (;;) {
        times(&buf);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Uso: %s <num_hijos> <segundos> <reducir_prioridad(0/1)>\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    int n_hijos = atoi(argv[1]);
    int segundos = atoi(argv[2]);
    int reducir = atoi(argv[3]);
    
    pid_t hijos[n_hijos];
    
    struct sigaction sa;
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGTERM, &sa, NULL) == -1) {
        perror("sigaction");
        return EXIT_FAILURE;
    }
    
    for (int i = 0; i < n_hijos; i++) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            return EXIT_FAILURE;
        }
        
        if (pid == 0) { 
            clock_gettime(CLOCK_MONOTONIC, &child_start);
            
            if (reducir == 1) {
                int nice_val = i;
                if (setpriority(PRIO_PROCESS, 0, nice_val) == -1) {
                    perror("setpriority");
                }
            }
            
            busywork();
            exit(EXIT_SUCCESS); 
        }
        
        hijos[i] = pid;
    }
    
    if (segundos > 0) {
        sleep((unsigned int)segundos);  
    } else {
        while (1) {
            pause();
        }
    }
    
    for (int j = 0; j < n_hijos; j++) {
        kill(hijos[j], SIGTERM);
    }
    
    for (int k = 0; k < n_hijos; k++) {
        waitpid(hijos[k], NULL, 0);
    }
    
    return EXIT_SUCCESS;
}