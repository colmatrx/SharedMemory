#include<stdio.h>
#include<stdlib.h>
#include "config.h"


int main(int argc, char *argv[]){

    int shared_memory_id;
    char *shared_memory_address;

    if (argc == 1){
        perror("\nrunsim: Error: Missing command line argument. Provide number of licenses\nUsage -> runsim n; where n = number of licenses.");   //use of perror
        exit(1);
    }

    if (argc > 2){       //test if more than 2 command line arguments are provided
        perror("\nrunsim: Error: Too many command line arguments.\nUsage -> runsim n; where n = number of licenses.");      //use of perror
        exit(1);
    }

    if (atoi(argv[1]) > max_number_of_processes){     //test if number of licenses is more than 20. atoi() converts char to integer
        perror("\nrunsim: Error: Maximum of number of licenses/processes allowed is 20.");    //use of perror
        exit(1);
    }

    if ((shared_memory_id = shmget(shared_memory_key, 100, IPC_CREAT|0666)) == -1)   //call to create a shared memory segment with shmget() with permissions 0666
        perror("\nrunsim: Error: shmget call failed."); //error checking shmget() call

    if ((shared_memory_address = shmat(shared_memory_id, NULL, 0)) == (char *) -1)  //call to shmat() to return the memory address of the shared_memory_id
        perror("\nrunsim: Error: shmat call failed.");  //error checking shmat() call

    printf("\nShared memory has been created and attached.\nKey is %x. ID is %d. Address is %u.\nNow sleeping for 20 seconds\n", shared_memory_key, shared_memory_id, shared_memory_address);

    sleep(20);

    if (shmctl(shared_memory_id, IPC_RMID, NULL) != 0)      //shmctl() marks the shared_memory_id for destruction so it can be deallocated from memory after no process is using it
        perror("\nrunsim: Error: shmctl() call failed. Segment cannot be marked for destruction"); //error checking shmctl() call

    printf("\nShared memory %d is now marked for destruction when no process is using it. Verify by the ipcs command. Now sleeping for 20 seconds\n", shared_memory_id);

    sleep(20);

    printf("\nShared memory will now be detached after sleeping for 20 seconds\n");

    sleep(20);

    if ((shmdt(shared_memory_address)) == -1)       //call to shmdt() to detach from the shared memory address
        perror("\nrunsim: Error: shmdt call failed");

    printf("\nShared memory was successfully detached and now sleeping for 20 seconds\n");

    sleep(20);

    return 0;
}