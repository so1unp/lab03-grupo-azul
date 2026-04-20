#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>      // times()
#include <unistd.h>

int busywork(void)
{
    struct tms buf;
    for (;;) {
        times(&buf);
    }
}

int main(int argc, char *argv[])
{
    int pid = 0;
    for(int i = 0; i < 2; i ++) {
        pid = fork();
        if(pid == 0) {
            printf("Hijo, pid: %d\n", pid);
        } else {
            printf("Padre, pid: %d\n", pid);
        }
    }

    exit(EXIT_SUCCESS);
}
