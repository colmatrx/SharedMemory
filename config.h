#define max_number_of_processes 20  //maximum number of processes/licenses allowed
#define appication_wait_timeout 100 //time for the parent process to wait before killing all child processes.
#define shared_memory_key 32714

void testsim(int sleepTime, int repeatFactor);

void logmsg(const char *msg);

char *logeventtime(void);

int getlicense(void);

int returnlicense(void);

int initlicense(void);

void addtolicenses(int n);

void removelicenses(int n);
