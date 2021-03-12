#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <math.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>

//constants
//shared memory keys
const key_t sharedKey;
const key_t mutexKey;
const key_t emptyKey;
const key_t fullKey;
const int QUEUE_SIZE = 25; //size of the queue
const int MAX_PROCESS = 19;
const int BUFFER = 8192;
const char* producerProg = "./produce";
const char* consumerProg = "./consume";
const char* writeLog = "./Monitor.log";

//state to decide critical section processing
enum state {wait, want, in};

//structures used to store information used in shared memory
struct itemPointer {
  int size;
  int nextItem;
  int currentItem;
};
struct itemInfo {
  int value;
  bool ready;
};
struct shmseg {
  int write;
  int read;
  int center;
};

// this variable is used to hold the returned segment identifier
int shm_id;
//used to specify where the page is attached
char* shm_addr;

//opterr defined here as extern variable
extern int opterr;
