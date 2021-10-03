#include<stdio.h>
#include<stdlib.h>
#include "config.h"

void siginthandler(int sig);

void timeouthandler(int sig);

int shared_memory_id; int d; int forkid; int forkcount;
int pid[max_number_of_processes]; int runcount;
char *shared_memory_address; char buff[200] = "\0";
char successstring[200] = "\0"; char *successtime;



int main(int argc, char *argv[]){

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

    runcount = atoi(argv[1]);

    printf("\nShared memory has been created and attached.\nKey is %x. ID is %d. Address is %p.\nNow sleeping for 20 seconds\n", shared_memory_key, shared_memory_id, shared_memory_address);

    printf("\nenter a string\n");

    read(0, buff, sizeof(buff)); //reading from the keyboard into the buff
    
    memcpy(shared_memory_address, buff, sizeof(buff)); //cpoies content of buff memory location to shared_ memory location

    for (forkcount = 0; forkcount < atoi(argv[1]); forkcount++){    //forking multiple times

        pid[forkcount] = fork();

        if (pid[forkcount] < 0){
        perror("\nrunsim: Error: fork() failed!\n");
        exit(1);
        }

        if (pid[forkcount] == 0){       //Child process
        
        printf("\nThis is Child Process ID %d\n", getpid());
        sleep(5); 
        execl("./testsim", "./testsim", "2", "3", NULL); //how to use execl to execute testsim. exec will not allow execution of codes after this line when it returns
        printf("\nend of child process\n");
        return 0; //return so that the child does not continue to execute codes after the for loop

        }

    }   

    //In Parent process

            //for (forkcount = 0; forkcount < atoi(argv[1]); forkcount++)
              //  wait(NULL);

            signal(SIGINT, siginthandler);  //handles Ctrl+C signal inside the parent process
            signal(SIGALRM, timeouthandler); //handles the timeout signal
            alarm(appication_wait_timeout); //fires timeout alarm after 100 seconds

            printf("\nThis is parent process ID %d\n", getpid());
            printf("\nWaiting for child processes signal\n");
            wait(0);

            printf("\nParent has stopped waiting because Child processes are now done\n");

            if ((shmdt(shared_memory_address)) == -1)       //call to shmdt() to detach from the shared memory address
                perror("\nrunsim: Error: Shared memory cannot be detached\n");

            printf("\nShared memory was successfully detached by Parent\n");

            if (shmctl(shared_memory_id, IPC_RMID, NULL) != 0)      //shmctl() marks the shared_memory_id for destruction so it can be deallocated from memory after no process is using it
                perror("\nrunsim: Error: shmctl() call failed. Segment cannot be marked for destruction\n"); //error checking shmctl() call

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

    for (forkcount = 0; forkcount < runcount; forkcount++)
        kill(pid[forkcount], SIGKILL); //sending a kill signal to the child processes to forcefully terminate them after Ctrl+C is received

    if ((shmdt(shared_memory_address)) == -1)       //call to shmdt() to detach from the shared memory address
            perror("\nrunsim: Error: Shared memory cannot be detached\n");

        printf("\nShared memory was successfully detached by Parent\n");

        if (shmctl(shared_memory_id, IPC_RMID, NULL) != 0)      //shmctl() marks the shared_memory_id for destruction so it can be deallocated from memory after no process is using it
            perror("\nrunsim: Error: shmctl() call failed. Segment cannot be marked for destruction\n"); //error checking shmctl() call

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

    for (forkcount = 0; forkcount < runcount; forkcount++)
        kill(pid[forkcount], SIGKILL); //sending a kill signal to the child processes to forcefully terminate them after timeout

    if ((shmdt(shared_memory_address)) == -1)       //call to shmdt() to detach from the shared memory address
            perror("\nrunsim: Error: Shared memory cannot be detached\n");

        printf("\nShared memory was successfully detached by Parent\n");

        if (shmctl(shared_memory_id, IPC_RMID, NULL) != 0)      //shmctl() marks the shared_memory_id for destruction so it can be deallocated from memory after no process is using it
            perror("\nrunsim: Error: shmctl() call failed. Segment cannot be marked for destruction\n"); //error checking shmctl() call

        printf("\nShared memory %d is now marked for destruction by Parent when no process is using it. Verify by the ipcs command.\n", shared_memory_id);

    log_time = logeventtime();

    snprintf(errorstring, sizeof(errorstring), "runsim timed out on\t%s", log_time);

    logmsg(errorstring); //logs error message before terminating parent process

    kill(getpid(), SIGTERM); //parent process terminating itself
    
    exit(1);

}