#ifndef SHAREDSTUFF
#define SHAREDSTUFF

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
const key_t SHARED = 0x12345;
const key_t MUTEX = 0x12345;
const key_t EMPTY = 0x12346;
const key_t FULL = 0x12347;
const int QUEUE_SIZE = 25; //size of the queue
const int MAX_PROCESS = 19;
const int BUFFER = 8192;
const char* producerProg = "./produce";
const char* consumerProg = "./consume";
const char* writeLog = "./Monitor.log";
extern int opterr;

// this variable is used to hold the returned segment identifier
int shm_id;
//used to specify where the page is attached
char* shm_addr;
//state to decide critical section processing
enum state {waiting, wantin, inside};

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

std::string getTheTime(const char* prePendString) {
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[10];
    
    // Get time
    time (&rawtime);
    timeinfo = localtime (&rawtime);

    // Format time for HH:MM:SS
    strftime (buffer,80,"%T",timeinfo);

    std::string strReturn = prePendString;
    strReturn.append(buffer);
    return strReturn;
}

//since there is no easy way to get a string from a int in C++
//this function converts an int value into a string
std::string getString(const int nVal) {
    int length = snprintf( NULL, 0, "%d", nVal);
    char* sDep = (char*)malloc( length + 1 );
    snprintf( sDep, length + 1, "%d", nVal);
    std::string strFinalVal = sDep;                    
    free(sDep);
    return strFinalVal;
}

#endif
