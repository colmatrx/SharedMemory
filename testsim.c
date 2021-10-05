#include<stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include<sys/types.h>
#include<unistd.h>
#include<string.h>
#include<strings.h>
#include<time.h>
#include<sys/wait.h>
#include<signal.h>
#include<stdlib.h>
void testsim(int, int);

int shared_memoryid; 
char *shared_memoryaddress;
int shared_memory_key = 32714;



int main(int argc, char *argv[]){

    long int arg1, arg2;

    arg1 = strtol(argv[1], NULL, 10); arg2 = strtol(argv[2], NULL, 10);

    printf("\nexecuting testsim\n");

    if ((shared_memoryid = shmget(shared_memory_key, 100, 0666)) == -1){   //call to create a shared memory segment with shmget() with permissions 0666
        perror("\nrunsim: Error: In testsim application: shmget call failed."); //error checking shmget() call
        exit(1);
    }

    if ((shared_memoryaddress = shmat(shared_memoryid, NULL, 0)) == (char *) -1){  //call to shmat() to return the memory address of the shared_memory_id
        perror("\nrunsim: Error: In testsim application: shmat call failed.\n");  //error checking shmat() call
        exit(1);
    }
    printf("\nContent of Shared Memory is %s", shared_memoryaddress);

    testsim(arg1, arg2 ); //takes command line args sleeptime and repeatfactor

    if ((shmdt(shared_memoryaddress)) == -1){       //call to shmdt() to detach from the shared memory address
        perror("\nrunsim: Error: In testsim application: Shared memory cannot be detached\n");
        exit(1);
    }

    printf("\nProcess %d completed execution\n", getpid());

    printf("\nShared memory was successfully detached from by Child process %d\n", getpid());

    return 0;

}