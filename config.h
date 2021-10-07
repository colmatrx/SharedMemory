#define max_number_of_processes 20  //maximum number of processes/licenses allowed
#define appication_wait_timeout 100 //time for the parent process to wait before killing all child processes.
#define shared_memory_key 32720
#define shmid_number_key 32715
#define shmid_choosing_key 32716

/*Author Idris Adeleke CS4760 Project 2 - Concurrent Linux Programming and SHared Memory*/

void testsim(int sleepTime, int repeatFactor);

void logmsg(const char *msg);

char *logeventtime(void);

int getlicense(void);

int returnlicense(void);

int initlicense(void);

void addtolicenses(int n);

void removelicenses(int n);
