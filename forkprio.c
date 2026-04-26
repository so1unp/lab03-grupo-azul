#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>      // times()
#include <unistd.h>
#include <signal.h>        // sigaction()
#include <sys/wait.h>      // wait()



void manejador_kill(int sig) {
    struct tms buf; times(&buf);
    printf("Child %d (nice %2d) : \t%3li\n", getpid(), getpriority(), buf);
    exit(EXIT_SUCCESS);
}


int busywork(void)
{
    struct tms buf;
    for (;;) {
        times(&buf);
    }
}

int main(int argc, int *argv[])
{   

    if (argc != 3) {
        fprintf(stderr, "Uso: %s [s|f] origen destino\n", argv[0]);
        return EXIT_FAILURE;
    }
    

    int priority = argv[2];
    int childsWants = argv[0];
    int childsId[childsWants];

    for (int i = 0; i < childsWants; i++ ) {
        childsId[i] = fork();
        if (childsId[i] == 0) {
            if(priority){
                if (i>19)
                {
                    nice(19);    /* code */
                }else{
                    nice(i);
                }
                                 
            } 
             struct sigaction sa;

            sa.sa_handler = manejador_kill; // Asignar la función
            sigemptyset(&sa.sa_mask);       // No bloquear otras señales durante el manejo
            sa.sa_flags = 0;                // Configuración por defecto

            if (sigaction(SIGTERM, &sa, NULL) == -1) {

                perror("Error registrando sigaction");
                return  1;
            }

            busywork();
            exit(EXIT_SUCCESS);
            
           // i = childsWants; Para salir del bucle
        }
    }
    if (argv[1] > 0 ){

        sleep(argv[1]);
            

    }else{
        pause();
    }
    for (int i = 0; i < childsWants; i++)
        {
            kill(childsId[i], SIGTERM);
        }
    exit(EXIT_SUCCESS);
}
