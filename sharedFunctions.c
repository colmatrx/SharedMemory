#include"config.h"

time_t  msgtime;

char logstring[200] = "\0";


void testsim(int sleepTime, int repeatFactor){      //textsim() sleeps for sleepTime seconds in a loop counted by repeatFactor

    int count = 0; char *logtime;

    for (count = 0; count < repeatFactor; count++){

        printf("\nSleeping for %d seconds\n", sleepTime);

        sleep(sleepTime);

        time(&msgtime);

        logtime = ctime(&msgtime);

        snprintf(logstring, sizeof(logstring), "PID %d\tIteration %d of %d\t%s", getpid(), count+1, repeatFactor, logtime); //concatenates the parameters as a single string

        logmsg(logstring);   

    }       


}

void logmsg(const char *msg){       //this function is for writing to logfile

    FILE *filedescriptor;

    filedescriptor = fopen("logfile.log", "a"); //open logfile in append mode

    if (filedescriptor == NULL){
        perror("\nrunsim: Error: Log File cannot be opened\n");
        exit(1);
    }

    fputs(msg, filedescriptor);    //writes log message to file
    fclose(filedescriptor);
       

}