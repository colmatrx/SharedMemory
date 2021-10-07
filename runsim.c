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

/*Author Idris Adeleke CS4760 Project 2 - Concurrent Linux Programming and SHared Memory*/
//This is the main runsim application

//function declaration and global variable block

void siginthandler(int sig);

void timeouthandler(int sig);

int maximum(int *); 

int getlicense(void);

int initlicense(void);

int returnlicense(void);

void removelicenses(int n);

char successstring[1024] = "\0"; char *successtime;

int count, forkcount; long int nlicense;

int pid[max_number_of_processes];

int numberarray[max_number_of_processes]; int choosingarray[max_number_of_processes];

int shared_memory_id; int shmid_choosing; int shmid_number; int turnnumber;

char *shared_memory_address;    int *choosing; int *number;


int main(int argc, char *argv[]){

    //In Parent process

    char filestore[2048]; char *execlargv1, *execlargv2, *execlarg; 
 
    if (argc == 1){     //testing if there is any command line argument
        perror("\nrunsim: Error: Missing command line argument. Provide number of licenses\nUsage -> runsim n; where n = number of licenses.");   //use of perror
        exit(1);
    }

    if (argc > 2){       //test if more than 2 command line arguments are provided
        perror("\nrunsim: Error: Too many command line arguments.\nUsage -> runsim n; where n = number of licenses.");      //use of perror
        exit(1);
    }

    nlicense = strtol(argv[1], NULL, 10); //copies command line license argument into global variable nlicense

    printf("\nNumber of Licenses is(are) %d", nlicense);    //prints out the number of licenses provided

    if ((nlicense > max_number_of_processes) || (nlicense <= 0)){     //tests if number of licenses is less than or equal to zero, or is more than 20. 
        perror("\nrunsim: Error: Minimun number of licenses allowed is 1.\nMaximum of number of licenses/processes allowed is 20.");    //use of perror
        exit(1);
    }

    printf("\nInitializing Shared memory with number of licenses\n");

    //sleep(5);

    initlicense();  //parent runsim process calls this function to initialize the shared memory with the number of licenses

    printf("\nShared memory initialized with  %d licenses", nlicense);

    number[forkcount] = 0;  //populating the turn index of the parent runsim
    choosing[forkcount] = 0;    //populating the choosing index of the parent runsim

    //sleep(2);
    
    printf("\nLicense shared memory has been created and attached.\nKey is %x. ID is %d. Address is %p.\n", shared_memory_key, shared_memory_id, shared_memory_address);

    //sleep(2);

    printf("\nNumber shared memory has been created and attached.\nKey is %x. ID is %d. Address is %p.\n", shmid_number_key, shmid_number, number);

    //sleep(2);

    printf("\nChoosing shared memory has been created and attached.\nKey is %x. ID is %d. Address is %p.\n", shmid_choosing_key, shmid_choosing, choosing);


    //start reading standard input here and fork based on the number of testsim lines in the input file
   
    while (fgets(filestore, 2048, stdin) != NULL){ //reads from the stdin 1 line at a time

            printf("\nObtaining license before forking a child process...\n");

            //sleep(5);

            while (getlicense() == 0);  //requesting a license before proceeding to fork a child process

            printf("\nLicense obtained in Parent process %d\n", getpid());

            removelicenses(1);      //call to decrement the number of available licenses after obtaining one above

            printf("\nLicense decremented by removelicense()\n");
            
            forkcount = forkcount + 1;

            execlarg = strtok(filestore, "  "); //using strtok() to extract the testsim argumments separated by a tab character
            
            execlargv1 = strtok(NULL, " ");    //using strtok() to extract the testsim argumments separated by a tab character

            execlargv2 = strtok(NULL, " ");    //using strtok() to extract the testsim argumments separated by a tab character
            
            pid[forkcount - 1] = fork();    //an array to store the process IDs

            if (pid[forkcount - 1] < 0){
                perror("\nrunsim: Error: fork() failed!\n");
                exit(1);
            }

            if (pid[forkcount - 1] == 0){       //This means a child process was created if true

                number[forkcount] = 0;      //populating the number index of the child process
                choosing[forkcount] = 0;    //populating the choosing index of the child process
       
                printf("\nThis is Child Process ID %d getting license\n", getpid());

                //sleep(5); 

                while (getlicense() == 0); //waiting while no license is available

                printf("\nThis is Child Process ID %d -> license obtained\n", getpid());

                removelicenses(1);  //call to decrement number of available licenses

                execl("./testsim", "./testsim", execlargv1, execlargv2, NULL); //how to use execl to execute testsim. exec will not allow execution of codes after this line when it returns

            }
        returnlicense();    //returning the license after successfully forking a child process
    }   

    //Back In Parent process

            signal(SIGINT, siginthandler);  //handles Ctrl+C signal inside the parent process
            signal(SIGALRM, timeouthandler); //handles the timeout signal
            alarm(appication_wait_timeout); //fires timeout alarm after 100 seconds defined in the config.h file

            printf("\nThis is parent process ID %d, number of child processes are = %d\n", getpid(), forkcount);
            printf("\nWaiting for child processes signal\n");

            for (count = 0; count < forkcount; count++){ //waiting for every process to finish ie. send SIGCHLD
                wait(0);
                printf("\n%d process completed\n", count+1);
                returnlicense();    //call return license when a child process finishes
            }

            printf("\nParent has stopped waiting because Child processes are now done\n");

            printf("\nChecking on all licenses....All %s licenses were returned and now available\n", shared_memory_address);

             if ((shmdt(shared_memory_address)) == -1){       //call to shmdt() to detach from the shared memory address
                perror("\nrunsim: Error: In Parent process: nlicense Shared memory cannot be detached\n");
                exit(1);
             }

             if ((shmdt(choosing)) == -1){       //call to shmdt() to detach from the shared choosing address
                perror("\nrunsim: Error: In Parent process: Choosing Shared memory cannot be detached\n");
                exit(1);
             }
             
             if ((shmdt(number)) == -1){       //call to shmdt() to detach from the shared number address
                perror("\nrunsim: Error: In Parent process: number Choosing Shared memory cannot be detached\n");
                exit(1);
             }

             sleep(3);

            printf("\nShared memories were successfully detached by Parent\n");

            if (shmctl(shared_memory_id, IPC_RMID, NULL) != 0){      //shmctl() marks the shared_memory_id for destruction so it can be deallocated from memory after no process is using it
                perror("\nrunsim: Error: In Parent process: shmctl() call failed. Segment cannot be marked for destruction\n"); //error checking shmctl() call
                exit(1);
            }

            if (shmctl(shmid_choosing, IPC_RMID, NULL) != 0){      //shmctl() marks the shmid_choosing for destruction so it can be deallocated from memory after no process is using it
                perror("\nrunsim: Error: In Parent process: shmid_choosing shmctl() call failed. Segment cannot be marked for destruction\n"); //error checking shmctl() call
                exit(1);
            }

            if (shmctl(shmid_number, IPC_RMID, NULL) != 0){      //shmctl() marks the shmid_number for destruction so it can be deallocated from memory after no process is using it
                perror("\nrunsim: Error: In Parent process: shmid_number shmctl() call failed. Segment cannot be marked for destruction\n"); //error checking shmctl() call
                exit(1);
            }

            printf("\nShared memories %d %d %d are now marked for destruction by Parent when no process is using it. Verify by the ipcs command.\n", shared_memory_id,shmid_choosing, shmid_number);

            successtime = logeventtime();   //getting the completion time

            snprintf(successstring, sizeof(successstring), "runsim successfully executed on\t%s ", successtime);

            logmsg(successstring); //logs successful execution message before ending the parent process 
   
    return 0;
}


//signal handler blocks

void siginthandler(int sig){    //function to handle Ctrl+C signal interrupt

    char *log_time; char errorstring[1024] = "\0";

    printf("\nCtrl+C received. Aborting Child and Parent Processes..\n");

    for (count = 0; count < forkcount; count++)
        kill(pid[count], SIGKILL); //sending a kill signal to the child processes to forcefully terminate them after Ctrl+C is received

    if ((shmdt(shared_memory_address)) == -1) {      //call to shmdt() to detach from the shared memory address
            perror("\nrunsim: Error: Signal Handler: Shared memory cannot be detached\n");
            exit(1);
    }

        printf("\nShared memory was successfully detached by Parent\n");

    if (shmctl(shared_memory_id, IPC_RMID, NULL) != 0){      //shmctl() marks the shared_memory_id for destruction so it can be deallocated from memory after no process is using it
            perror("\nrunsim: Error: Signal Handler: shmctl() call failed. Segment cannot be marked for destruction\n"); //error checking shmctl() call
            exit(1);
    }

    if ((shmdt(choosing)) == -1){       //call to shmdt() to detach from the shared choosing address
                perror("\nrunsim: Error: In Parent process: Choosing Shared memory cannot be detached\n");
                exit(1);
    }

    if (shmctl(shmid_choosing, IPC_RMID, NULL) != 0){      //shmctl() marks the shmid_choosing for destruction so it can be deallocated from memory after no process is using it
            perror("\nrunsim: Error: In Parent process: shmid_choosing shmctl() call failed. Segment cannot be marked for destruction\n"); //error checking shmctl() call
            exit(1);
    }

             
    if ((shmdt(number)) == -1){       //call to shmdt() to detach from the shared number address
            perror("\nrunsim: Error: In Parent process: number Choosing Shared memory cannot be detached\n");
            exit(1);
    }

    
    if (shmctl(shmid_number, IPC_RMID, NULL) != 0){      //shmctl() marks the shmid_number for destruction so it can be deallocated from memory after no process is using it
            perror("\nrunsim: Error: In Parent process: shmid_number shmctl() call failed. Segment cannot be marked for destruction\n"); //error checking shmctl() call
            exit(1);
    }

        printf("\nShared memories %d %d %d are now marked for destruction by Parent when no process is using it. Verify by the ipcs command.\n", shared_memory_id,shmid_choosing, shmid_number);

    log_time = logeventtime();  //getting time of signal interruption

    snprintf(errorstring, sizeof(errorstring), "runsim terminated by Ctrl+C on\t%s", log_time); //concatenating the errorstring to be written to log file

    logmsg(errorstring); //logs error message before terminating parent process

    kill(getpid(), SIGTERM); //parent process terminating itself
    
    exit(1);

}

void timeouthandler(int sig){   //this function is called if the program times out after 100 seconds

    char *log_time; char errorstring[1024] = "\0";

    printf("\nrunsim timeout. Aborting Child and Parent Processes..\n");

    for (count = 0; count < forkcount; count++)
        kill(pid[count], SIGKILL); //sending a kill signal to the child processes to forcefully terminate them after timeout

    if ((shmdt(shared_memory_address)) == -1){       //call to shmdt() to detach from the shared memory address
            perror("\nrunsim: Error: In Timeout handler: Shared memory cannot be detached\n");
            exit(1);
    }

    if ((shmdt(choosing)) == -1){       //call to shmdt() to detach from the shared choosing address
            perror("\nrunsim: Error: In Parent process: Choosing Shared memory cannot be detached\n");
            exit(1);
    }
             
    if ((shmdt(number)) == -1){       //call to shmdt() to detach from the shared number address
            perror("\nrunsim: Error: In Parent process: number Choosing Shared memory cannot be detached\n");
            exit(1);
    }


        printf("\nShared memories were successfully detached by Parent\n");

    if (shmctl(shared_memory_id, IPC_RMID, NULL) != 0){      //shmctl() marks the shared_memory_id for destruction so it can be deallocated from memory after no process is using it
            perror("\nrunsim: Error: In Timeout handler: shmctl() call failed. Segment cannot be marked for destruction\n"); //error checking shmctl() call
            exit(1);
    }

    if (shmctl(shmid_choosing, IPC_RMID, NULL) != 0){      //shmctl() marks the shmid_choosing for destruction so it can be deallocated from memory after no process is using it
                perror("\nrunsim: Error: In Parent process: shmid_choosing shmctl() call failed. Segment cannot be marked for destruction\n"); //error checking shmctl() call
                exit(1);
    }

    if (shmctl(shmid_number, IPC_RMID, NULL) != 0){      //shmctl() marks the shmid_number for destruction so it can be deallocated from memory after no process is using it
            perror("\nrunsim: Error: In Parent process: shmid_number shmctl() call failed. Segment cannot be marked for destruction\n"); //error checking shmctl() call
            exit(1);
    }


    printf("\nShared memories %d %d %d are now marked for destruction by Parent when no process is using it. Verify by the ipcs command.\n", shared_memory_id,shmid_choosing, shmid_number);
    
    log_time = logeventtime();  //getting event time

    snprintf(errorstring, sizeof(errorstring), "runsim timed out on\t%s", log_time);

    logmsg(errorstring); //logs error message before terminating parent process

    kill(getpid(), SIGTERM); //parent process terminating itself
    
    exit(1);

}

int getlicense(void){       //returns 1 for license available, and 0 for no license available

    //choosing[forkcount] = 1; //1 for true and 0 for false

   // number[forkcount] = 1 + maximum(number);

   // printf("\nthis is process %d getting license; index is %d ; turn is %d\n", getpid(), forkcount, number[forkcount]);

    //choosing[forkcount] = 0;    //1 for true and 0 for false

   // for(int j = 0; j < 10; j++){
        
    //    while(choosing[j]);

      // while ((number[j] !=0) && (number[j] < number[forkcount] || (number[j] == number[forkcount] && j < forkcount)));     

   // }

    //start of critical section

    long int license = 0; char textlicense[20];

    license = strtol(shared_memory_address, NULL, 10); //read shared memory content

  //  number[forkcount] = 0;

    //end of critical section

    if (license <= 0 )
        return 0;
      
    else
        return 1;
    
}


int initlicense(void){      //this function initializes the shared memories and copies the number of license in the appropriate one

    char textlicense[20];

    if ((shared_memory_id = shmget(shared_memory_key, 100, IPC_CREAT|0666)) == -1){   //call to create a shared memory segment with shmget() with permissions 0666
        perror("\nrunsim: Error: In initlicense(): shmget call failed."); //error checking shmget() call
        exit(1);
    }

    if ((shared_memory_address = shmat(shared_memory_id, NULL, 0)) == (char *) -1){  //call to shmat() to return the memory address of the shared_memory_id
        perror("\nrunsim: Error: In initlicense(): shmat call failed.");  //error checking shmat() call
        exit(1);
    }

    if ((shmid_choosing = shmget(shmid_choosing_key, sizeof(numberarray), IPC_CREAT|0666)) == -1){   //call to create a shared memory segment with shmget() with permissions 0666
        perror("\nrunsim: Error: In initlicense(): shmget call to choosing failed."); //error checking shmget() call
        exit(1);
    }

    if ((choosing = (int *)shmat(shmid_choosing, NULL, 0)) == (int *) -1){  //call to shmat() to return the memory address of the shmid_choosing
        perror("\nrunsim: Error: In initlicense(): shmat call to choosing failed.");  //error checking shmat() call
        exit(1);
    }

    if ((shmid_number = shmget(shmid_number_key, sizeof(choosingarray), IPC_CREAT|0666)) == -1){   //call to create a shared memory segment with shmget() with permissions 0666
        perror("\nrunsim: Error: In initlicense(): shmget call to number failed."); //error checking shmget() call
        exit(1);
    }

    if ((number = (int *)shmat(shmid_number, NULL, 0)) == (int *) -1){  //call to shmat() to return the memory address of the shmid_number
        perror("\nrunsim: Error: In initlicense(): shmat call to number failed.");  //error checking shmat() call
        exit(1);
    }

    sprintf(textlicense, "%d", nlicense); //converts nlicense to string so we can copy to shared memory

    strcpy(shared_memory_address, textlicense); //populate the shared memory with nlicense value

    return 0;
}


int returnlicense(void){    //increments the number of available licenses.
                            //called by parent to return the license after child process execution

  //  choosing[forkcount] = 1;

  //  number[forkcount] = 1 + maximum(number);

  //  printf("\nthis is process %d returning license; turn is %d\n", getpid(), number[forkcount]);

 //   choosing[forkcount] = 0;

  //  for(int j = 0; j < max_number_of_processes; j++){
        
 //       while(choosing[j]);

 //       while ((number[j] !=0) && (number[j] < number[forkcount] || (number[j] == number[forkcount] && j < forkcount)));      

//    }

    //beginning of critical section

    long int license = 0; char textlicense[20];

    license = strtol(shared_memory_address, NULL, 10); //converts shared memory content to integer

    license =license + 1;

    sprintf(textlicense, "%d", license);

    strcpy(shared_memory_address,textlicense);

    printf("\n1 license returned\n", shared_memory_address);

    //end of critical section

//    number[forkcount] = 0;

    return 0;
}


void removelicenses(int n){ //removes n from number of licenses

  //  choosing[forkcount] = 1;

  //  number[forkcount] = 1 + maximum(number);

  //  printf("\nthis is process %d returning license; index is %d\n", getpid(), forkcount);

 //  choosing[forkcount] = 0;

 //  for(int j = 0; j < max_number_of_processes; j++){
        
  //      while(choosing[j]);

  //      while ((number[j] !=0) && (number[j] < number[forkcount] || (number[j] == number[forkcount] && j < forkcount)));      

  //  }
    
    //Start of critical section

    long int license = 0; char textlicense[20];
    
    license = strtol(shared_memory_address, NULL, 10); //converts shared memory content to integer

    license = license - n;

    sprintf(textlicense, "%d", license);

    strcpy(shared_memory_address,textlicense);
    
    printf("\n%s licenses available\n", shared_memory_address);

    //end of critical section
    
   // number[forkcount] = 0;    
}


void addtolicenses(int n){  //this function adds n to the number of licenses. It is never used in this program so it is not considered a critical section in this case

    long int license = 0; char textlicense[20];

    license = strtol(shared_memory_address, NULL, 10); //converts shared memory content to integer

    license = license + n;

    sprintf(textlicense, "%d", license);

    strcpy(shared_memory_address,textlicense);

    printf("\n%s licenses have been added\n", shared_memory_address);

}


int maximum (int *number){  //this function is called by processes to find the maximum number in the number array

    int i = 0;
    int maxnumber = 0;
    for(i = 0; i<max_number_of_processes; i++){

        if (number[i] > maxnumber)  //comparing the content of process_number array
            maxnumber = number[i];
    }
    return maxnumber;
}