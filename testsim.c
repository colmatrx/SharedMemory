#include"config.h"

int shared_memory_id; 
char *shared_memory_address;


int main(int argc, char *argv[]){

    printf("\nexecuting testsim\n");

    if ((shared_memory_id = shmget(shared_memory_key, 100, 0666)) == -1)   //call to create a shared memory segment with shmget() with permissions 0666
        perror("\nrunsim: Error: shmget call failed."); //error checking shmget() call

    if ((shared_memory_address = shmat(shared_memory_id, NULL, 0)) == (char *) -1)  //call to shmat() to return the memory address of the shared_memory_id
        perror("\nrunsim: Error: shmat call failed.");  //error checking shmat() call

    printf("\nContent of Shared Memory is %s", shared_memory_address);

    testsim(atoi(argv[1]), atoi(argv[2])); //takes command line args sleeptime and repeatfactor

    if ((shmdt(shared_memory_address)) == -1)       //call to shmdt() to detach from the shared memory address
        perror("\nrunsim: Error: Shared memory cannot be detached\n");

    printf("\nShared memory was successfully detached\n");

    if (shmctl(shared_memory_id, IPC_RMID, NULL) != 0)      //shmctl() marks the shared_memory_id for destruction so it can be deallocated from memory after no process is using it
        perror("\nrunsim: Error: shmctl() call failed. Segment cannot be marked for destruction\n"); //error checking shmctl() call

    printf("\nShared memory %d is now marked for destruction when no process is using it. Verify by the ipcs command.\n", shared_memory_id);

    return 0;


}