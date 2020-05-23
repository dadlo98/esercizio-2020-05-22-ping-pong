#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <errno.h>

#define MAX_VALUE 1000000

int main(int argc, char * argv[]) {
    int pipeA[2];
    int pipeB[2];

    if(pipe(pipeA) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    if(pipe(pipeB) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    int counter = 0;
    int res;

    switch(fork()) {
        case -1:
            perror("fork()");
            exit(EXIT_FAILURE); 
        
        case 0:
            close(pipeA[1]); //chiudo scrittura su pipeA per il figlio
            close(pipeB[0]); //chiudo lettura su pipeB per il figlio

            while(counter != MAX_VALUE){

                if((res=read(pipeA[0], &counter, sizeof(counter))) == -1) {
                    perror("read");
                    exit(EXIT_FAILURE);
                }

                counter++;

                if((res=write(pipeB[1], &counter, sizeof(counter))) == -1) {
                    perror("write");
                    exit(EXIT_FAILURE);
                }
            }
            
            //chiudo i rimanenti fd delle pipes
            close(pipeA[0]);
            close(pipeB[1]);

            exit(0);
        
        default:
            close(pipeA[0]); //chiudo lettura su pipeA per il padre
            close(pipeB[1]); //chiudo scrittura su pipeB per il padre

            while(counter != MAX_VALUE){

                if((res=write(pipeA[1], &counter, sizeof(counter))) == -1) {
                    perror("write");
                    exit(EXIT_FAILURE);
                }

                if((res=read(pipeB[0], &counter, sizeof(counter))) == -1) {
                    perror("read");
                    exit(EXIT_FAILURE);
                }

                counter++;
            }

            //chiudo i rimanenti fd delle pipes
            close(pipeA[1]);
            close(pipeB[0]);

            if(wait(NULL) == -1) {
                perror("wait");
                exit(EXIT_FAILURE);
            }
    }

    printf("\nFinal value of counter: %d", counter);
    printf("\nExpected final value of counter: %d\n", MAX_VALUE);

    return 0;
}
