#include<stdio.h>
#include<stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include<sys/types.h>

#define max_number_of_processes 20  //maximum number of processes/licenses allowed
#define appication_wait_timeout 100 //time for the parent process to wait before killing all child processes.
#define shared_memory_key 32714