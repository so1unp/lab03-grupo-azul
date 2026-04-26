#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>      // times()
#include <time.h>         // clock_gettime()
#include <unistd.h>
#include <signal.h>        // sigaction()
#include <sys/wait.h>      // wait()
#include <sys/resource.h> // getpriority(), setpriority()



void manejador_kill(int sig) {
    (void)sig; // Evitar advertencia de variable no utilizada
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    long int ms = (long int)(now.tv_sec * 1000L + now.tv_nsec / 1000000L);
    printf("Child %d (nice %2d) : \t%ld ms\n", getpid(), getpriority(PRIO_PROCESS, 0), ms);
    exit(EXIT_SUCCESS);
}


int busywork(void)
{
    struct tms buf;
    for (;;) {
        times(&buf);
    }
}

int main(int argc, char *argv[])
{   
    struct sigaction sa;

    sa.sa_handler = manejador_kill; // Asignar la función
    sigemptyset(&sa.sa_mask);       // No bloquear otras señales durante el manejo
    sa.sa_flags = 0;                // Configuración por defecto

    if (sigaction(SIGTERM, &sa, NULL) == -1) {

        perror("Error registrando sigaction");
        return  1;
    }

    if (argc != 4) {
        fprintf(stderr, "Uso: %s [s|f] origen destino\n", argv[0]);
        return EXIT_FAILURE;
    }
    

    int priority = atoi(argv[3]);
    int childsWants = atoi(argv[1]);
    int childsId[childsWants];

    for (int i = 0; i < childsWants; i++ ) {
        childsId[i] = fork();
        if (childsId[i] == 0) {
            if(priority){
                if (i>19)
                {
                    setpriority(PRIO_PROCESS, 0, 19);    /* code */
                }else{
                    setpriority(PRIO_PROCESS, 0, i);
                }
                                 
            } 
                    
            busywork();
            exit(EXIT_SUCCESS);
            
           // i = childsWants; Para salir del bucle
        }
    }
    if (atoi(argv[2]) > 0 ){

        sleep((unsigned int)(atoi(argv[2])));
            

    }else{
        pause();
    }
    for (int i = 0; i < childsWants; i++)
        {
            kill(childsId[i], SIGTERM);
        }
    exit(EXIT_SUCCESS);
}
