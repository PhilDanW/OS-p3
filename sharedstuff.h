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

struct sembuf {
    int sem_num;
    int sem_op;
    int sem_flag;
};

class semaphores
{
    public:
        semaphores(key_t, bool, int = 1);
        ~semaphores();
        // Check if properly setup
        bool isInitialized() { return _isInitialized; };
        // Semaphore Wait
        void Wait();
        // Semaphore Signal
        void Signal();  
    private:
        bool _bCreator;
        int _semid;
        bool _isInitialized;
        struct sembuf structSemaBuf;
};

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

bool WriteToLog(std::string& logString, std::string LogFile)
{
    // Open a file to write
    std::ofstream logFile (LogFile.c_str(), std::ofstream::out | std::ofstream::app);
    if (logFile.is_open())
    {
        // Get the current local time
//        string 
        logFile << GetTimeFormatted("").c_str();
        logFile << " " << logString.c_str();
        logFile << std::endl;
        logFile.close();
        return true;
    }
    else
    {
        perror("Failed to write to the log");
        return false;
    }
}

std::string getString(const int nVal)
{
    int length = snprintf( NULL, 0, "%d", nVal);
    char* sDep = (char*)malloc( length + 1 );
    snprintf( sDep, length + 1, "%d", nVal);
    std::string strFinalVal = sDep;                    
    free(sDep);
    return strFinalVal;
}


#endif
