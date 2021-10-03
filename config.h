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


#define max_number_of_processes 20  //maximum number of processes/licenses allowed
#define appication_wait_timeout 100 //time for the parent process to wait before killing all child processes.
#define shared_memory_key 32714



void testsim(int sleepTime, int repeatFactor);

void logmsg(const char *msg);

char *logeventtime(void);
