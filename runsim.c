#include<stdio.h>
#include<stdlib.h>
#include "config.h"


int main(int argc, char *argv[]){

    int shared_memory_id; int d; int forkid;
    char *shared_memory_address; char buff[100] = "\0";

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

    printf("\nShared memory has been created and attached.\nKey is %x. ID is %d. Address is %p.\nNow sleeping for 20 seconds\n", shared_memory_key, shared_memory_id, shared_memory_address);

    printf("\nenter a string\n");

    read(0, buff, sizeof(buff)); //reading from the keyboard into the buff
    
    memcpy(shared_memory_address, buff, sizeof(buff)); //cpoies content of buff memory location to shared_ memory location

    forkid = fork();

    if (forkid < 0){
        perror("\nrunsim: Error: fork() failed!\n");
        exit(1);
    }

    if (forkid == 0){       //Child exec
        
        printf("\nThis is Child Process ID %d\n", getpid());
        //printf("\nContent of Shared memory is %s\n", shared_memory_address);
        sleep(10); //char *arr[] = {"./testsim", "3", "2", NULL}; execv(arr[0], arr); //how to use execv
        execl("./testsim", "./testsim", "2", "5", NULL); //how to use execl
        sleep(10);
        printf("\nend of child process\n");

    }

    else{           //Parent exec

        printf("\nStill in parent process ID %d\n", getpid());
        printf("\nwaiting for child process signal\n");
        wait(0);

    }

   
    return 0;
}