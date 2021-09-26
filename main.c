#include<stdio.h>
#include<stdlib.h>
#include "config.h"


int main(int argc, char *argv[]){

    if (argc == 1){
        perror("\nrunsim: Error: Missing command line argument. Provide number of licenses");   //use of perror
        exit(0);
    }

    if (argc > 2){       //test if more than 2 command line arguments are provided
        perror("\nrunsim: Error: Too many command line arguments.");      //use of perror
        exit(0);
    }

    if (atoi(argv[1]) > max_number_of_processes){     //test if number of licenses is more than 20. atoi() converts char to integer
        perror("\nrunsim: Error: Maximum of number of licenses/processes allowed is 20.");    //use of perror
        exit(0);
    }

    





    return 0;
}