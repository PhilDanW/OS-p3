//Philip Wright
//CMP4760 Project3
//this part of the project is a .h file that contains many shared variables, 
//structures, and helper functions
#ifndef SHAREDSTRUCTURES_H
#define SHAREDSTRUCTURES_H

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

// for arguement processing
extern int opterr;

// for critical section processing
enum state { idle, want_in, in_cs };

// Shared Memory structures
struct ProductHeader {
    int pNextQueueItem; // Point to next item for Producer
    int pCurrent;       // Point to next item for Consumer
    int QueueSize;   // Size of queue
};
struct ProductItem {
    bool readyToProcess;    // Ready to Process
    float itemValue;        // The actual Product 
};

const key_t KEY_SHMEM = ftok(".", 'q');  // Shared key
int shm_id; // Shared Mem ident
char* shm_addr;

// Product Semaphores
const key_t KEY_MUTEX = ftok(".", 'c');
const key_t KEY_EMPTY = ftok(".", 'b');
const key_t KEY_FULL = ftok(".", 'a');

struct shmseg {
   int cntr;
   int write_complete;
   int read_complete;
};

// The size of our product queue
const int PRODUCT_QUEUE_LENGTH = 20;

const char* ProducerProcess = "./producer";
const char* ConsumerProcess = "./consumer";

// Helper Functions

std::string GetTimeFormatted(const char* prePendString)
{
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

// Log file writing helper function
bool WriteLogFile(std::string& logString, std::string LogFile)
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
        perror("Unable to write to log file");
        return false;
    }
}

// Returns a string from an int
std::string GetStringFromInt(const int nVal)
{
    int length = snprintf( NULL, 0, "%d", nVal);
    char* sDep = (char*)malloc( length + 1 );
    snprintf( sDep, length + 1, "%d", nVal);
    std::string strFinalVal = sDep;                    
    free(sDep);
    return strFinalVal;
}

#endif
