#include<stdio.h>
#include<stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include<sys/types.h>
#include<unistd.h>
#include<string.h>
#include<strings.h>
#include<time.h>
#include<sys/wait.h>
#include<signal.h>
#include "config.h"

int forkcount;

int pid[max_number_of_processes];

char buff[200] = "\0";

char successstring[200] = "\0"; char *successtime;

void siginthandler(int sig);

void timeouthandler(int sig);

long int nlicense;

int getlicense(void);

int initlicense(void);

int returnlicense(void);

void removelicenses(int n);

int shared_memory_id;

char *shared_memory_address;

int main(int argc, char *argv[]){

    
    //In Parent process

    if (argc == 1){
        perror("\nrunsim: Error: Missing command line argument. Provide number of licenses\nUsage -> runsim n; where n = number of licenses.");   //use of perror
        exit(1);
    }

    if (argc > 2){       //test if more than 2 command line arguments are provided
        perror("\nrunsim: Error: Too many command line arguments.\nUsage -> runsim n; where n = number of licenses.");      //use of perror
        exit(1);
    }

    nlicense = strtol(argv[1], NULL, 10); //copies command line license into global variable nlicense

    if ((atoi(argv[1]) > max_number_of_processes) || (atoi(argv[1]) == 0)){     //test if number of licenses is more than 20. atoi() converts char to integer
        perror("\nrunsim: Error: Minimun number of licenses allowed is 1.\nMaximum of number of licenses/processes allowed is 20.");    //use of perror
        exit(1);
    }

    initlicense();
    
    printf("\nShared memory has been created and attached.\nKey is %x. ID is %d. Address is %p.\n", shared_memory_key, shared_memory_id, shared_memory_address);
   
    for (forkcount = 0; forkcount < nlicense; forkcount++){    //forking nlicense number of child processes

        pid[forkcount] = fork();

        if (pid[forkcount] < 0){
        perror("\nrunsim: Error: fork() failed!\n");
        exit(1);
        }

        if (pid[forkcount] == 0){       //In Child process
        
        printf("\nThis is Child Process ID %d getting license\n", getpid());

        sleep(5); 

        while (getlicense() == 0); //waiting while no license is available

        printf("\nThis is Child Process ID %d -> license obtained\n", getpid());

        removelicenses(1);

        execl("./testsim", "./testsim", "2", "3", NULL); //how to use execl to execute testsim. exec will not allow execution of codes after this line when it returns
        }

    }   

    //Back In Parent process

            signal(SIGINT, siginthandler);  //handles Ctrl+C signal inside the parent process
            signal(SIGALRM, timeouthandler); //handles the timeout signal
            alarm(appication_wait_timeout); //fires timeout alarm after 100 seconds

            printf("\nThis is parent process ID %d\n", getpid());
            printf("\nWaiting for child processes signal\n");
            for (forkcount = 0; forkcount < nlicense; forkcount++){ //waiting for every process to finish ie. send SIGCHLD
                wait(0);
                printf("\n%d process completed\n", forkcount+1);
                returnlicense();    //call return license to add to the number of licenses when a child process finishes
            }

            printf("\nParent has stopped waiting because Child processes are now done\n");

             if ((shmdt(shared_memory_address)) == -1){       //call to shmdt() to detach from the shared memory address
                perror("\nrunsim: Error: In Parent process: Shared memory cannot be detached\n");
                exit(1);
             }

            printf("\nShared memory was successfully detached by Parent\n");

            if (shmctl(shared_memory_id, IPC_RMID, NULL) != 0){      //shmctl() marks the shared_memory_id for destruction so it can be deallocated from memory after no process is using it
                perror("\nrunsim: Error: In Parent process: shmctl() call failed. Segment cannot be marked for destruction\n"); //error checking shmctl() call
                exit(1);
            }

            printf("\nShared memory %d is now marked for destruction by Parent when no process is using it. Verify by the ipcs command.\n", shared_memory_id);

            successtime = logeventtime();
            snprintf(successstring, sizeof(successstring), "runsim successfully executed on\t%s ", successtime);
            logmsg(successstring); //logs successful execution message before ending the parent process 
   
    return 0;
}


//signal handler blocks
void siginthandler(int sig){    //function to handle Ctrl+C signal interrupt

    char *log_time; char errorstring[200] = "\0";

    printf("\nCtrl+C received. Aborting Child and Parent Processes..\n");

    for (forkcount = 0; forkcount < nlicense; forkcount++)
        kill(pid[forkcount], SIGKILL); //sending a kill signal to the child processes to forcefully terminate them after Ctrl+C is received

    if ((shmdt(shared_memory_address)) == -1) {      //call to shmdt() to detach from the shared memory address
            perror("\nrunsim: Error: Signal Handler: Shared memory cannot be detached\n");
            exit(1);
    }

        printf("\nShared memory was successfully detached by Parent\n");

    if (shmctl(shared_memory_id, IPC_RMID, NULL) != 0){      //shmctl() marks the shared_memory_id for destruction so it can be deallocated from memory after no process is using it
            perror("\nrunsim: Error: Signal Handler: shmctl() call failed. Segment cannot be marked for destruction\n"); //error checking shmctl() call
            exit(1);
    }

        printf("\nShared memory %d is now marked for destruction by Parent when no process is using it. Verify by the ipcs command.\n", shared_memory_id);

    log_time = logeventtime();

    snprintf(errorstring, sizeof(errorstring), "runsim terminated by Ctrl+C on\t%s", log_time);

    logmsg(errorstring); //logs error message before terminating parent process

    kill(getpid(), SIGTERM); //parent process terminating itself
    
    exit(1);

}

void timeouthandler(int sig){

    char *log_time; char errorstring[200] = "\0";

    printf("\nrunsim timeout. Aborting Child and Parent Processes..\n");

    for (forkcount = 0; forkcount < nlicense; forkcount++)
        kill(pid[forkcount], SIGKILL); //sending a kill signal to the child processes to forcefully terminate them after timeout

    if ((shmdt(shared_memory_address)) == -1){       //call to shmdt() to detach from the shared memory address
            perror("\nrunsim: Error: In Timeout handler: Shared memory cannot be detached\n");
            exit(1);
    }

        printf("\nShared memory was successfully detached by Parent\n");

    if (shmctl(shared_memory_id, IPC_RMID, NULL) != 0){      //shmctl() marks the shared_memory_id for destruction so it can be deallocated from memory after no process is using it
            perror("\nrunsim: Error: In Timeout handler: shmctl() call failed. Segment cannot be marked for destruction\n"); //error checking shmctl() call
            exit(1);
    }

        printf("\nShared memory %d is now marked for destruction by Parent when no process is using it. Verify by the ipcs command.\n", shared_memory_id);

    log_time = logeventtime();

    snprintf(errorstring, sizeof(errorstring), "runsim timed out on\t%s", log_time);

    logmsg(errorstring); //logs error message before terminating parent process

    kill(getpid(), SIGTERM); //parent process terminating itself
    
    exit(1);

}

int getlicense(void){       //returns 1 for license available, and 0 for no license available

    long int license = 0;

    license = strtol(shared_memory_address, NULL, 10); //read shared memory content

    if (license <= 0)
        return 0;
    
    else        
        return 1;
}

int initlicense(void){

    char textlicense[20];

    if ((shared_memory_id = shmget(shared_memory_key, 100, IPC_CREAT|0666)) == -1){   //call to create a shared memory segment with shmget() with permissions 0666
        perror("\nrunsim: Error: In initlicense(): shmget call failed."); //error checking shmget() call
        exit(1);
    }

    if ((shared_memory_address = shmat(shared_memory_id, NULL, 0)) == (char *) -1){  //call to shmat() to return the memory address of the shared_memory_id
        perror("\nrunsim: Error: In initlicense(): shmat call failed.");  //error checking shmat() call
        exit(1);
    }

    sprintf(textlicense, "%d", nlicense); //converts nlicense to string so we can copy to shared memory

    strcpy(shared_memory_address, textlicense); //populate the shared memory with nlicense value

    return 0;
}

int returnlicense(void){    //increments the number of available licenses.
                            //called by parent to return the license after child process execution
    long int license = 0; char textlicense[20];
    license = strtol(shared_memory_address, NULL, 10); //converts shared memory content to integer
    license++;
    sprintf(textlicense, "%d", license);
    strcpy(shared_memory_address,textlicense);
    printf("\n1 license returned\n", shared_memory_address);
    return 0;
}

void removelicenses(int n){ //removes n from number of licenses

    long int license = 0; char textlicense[20];
    license = strtol(shared_memory_address, NULL, 10); //converts shared memory content to integer
    license = license - n;
    sprintf(textlicense, "%d", license);
    strcpy(shared_memory_address,textlicense);
    printf("\n%s licenses available\n", shared_memory_address);
    
}

void addtolicenses(int n){

    long int license = 0; char textlicense[20];
    license = strtol(shared_memory_address, NULL, 10); //converts shared memory content to integer
    license = license + n;
    sprintf(textlicense, "%d", license);
    strcpy(shared_memory_address,textlicense);
    printf("\n%s licenses have been added\n", shared_memory_address);

}