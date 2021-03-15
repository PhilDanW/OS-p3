#include <iostream>
#include <unistd.h>
#include "sharedstuff.h"
#include "semaphores.h"
#include <fstream>
using namespace std;

static void allocateMemory();
volatile sig_atomic_t gSignalStatus = 0;
void signal_handler(int signal)
{
    gSignalStatus = 1;
}

int main(int argc, char* argv[])
{
    signal(SIGINT, signal_handler);
    
    
    string myLog = argv[1];
    srand(time(NULL));
    int sleepTime = rand() % 10 + 1;
    
    int child = getpid();
    string myPid = getString(child);
    string strlog = "Producer's PID: ";
    strlog.append(myPid);
    strlog.append(" has started.");
    ofstream ofoutputFile (myLog, ios::app);
    if (myLog.is_open()) {
        ofoutputFile << getTheTime("").c_str();
                     << " " << strlog.c_str();
                     << std::endl;
        ofoutputFile.close();
    }
    else {
        perror("Failed to write to the log");
        return false;
    }
  
    semaphores s(MUTEX, false);
    semaphores n(EMPTY, false);
    semaphores e(FULL, false);
  
    allocateMemory();
  
    // Get the queue header
    struct itemPointer* head = (struct itemPointer*) (shm_addr);
    // Get our entire queue
    struct itemInfo* queue  = (struct itemInfo*) (shm_addr + sizeof(int) + sizeof(head));
  
    while(!gSignalStatus)
    {
        int sleepTime = rand() % 5 + 1;
        sleep(sleepTime);

        e.Wait();
        s.Wait();
      
        int myValue = ((1 + 2) * (3 + 4) * (5 + 6));
      
        queue[head->nextItem].value = myValue;
        queue[head->nextItem].ready = true;

        // Log what happened into System Log
        string myPID = getString(child); 
        string myItem = getString(head->nextItem);
        strlog = "Producer's PID: ";
        strlog.append(myPID);
        strlog.append(" put item in queue: ");
        strlog.append(myItem);
        ofstream ofoutputFile (myLog, ios::app);
    if (myLog.is_open()) {
        ofoutputFile << getTheTime("").c_str();
                     << " " << strlog.c_str();
                     << std::endl;
        ofoutputFile.close();
    }
    else {
        perror("Failed to write to the log");
        return false;
    }
      
        head->nextItem =(++head->nextItem) % head->size;

        s.Signal();
        n.Signal();

        return EXIT_SUCCESS;
    }
}

void allocateMemory() {
  
  shm_id = shmget(SHARED, 0, 0);
  if (shm_id == -1) {
      perror("consumer: Error: failed to find shm_id ");
      exit(EXIT_FAILURE);
  }

  // Get the size of the memory
  struct shmid_ds shmid_ds;
  shmctl(shm_id, IPC_STAT, &shmid_ds);
  size_t realSize = shmid_ds.shm_segsz;

  // use shmget to setup with the size of the memory
  shm_id = shmget(SHARED, realSize, 0);
  if (shm_id == -1) {
      perror("consumer: Error: failed to setup memory with shmget");
      exit(EXIT_FAILURE);
  }

  // attach the shared memory segment to our process's address space
  shm_addr = (char*)shmat(shm_id, NULL, 0);
  if (!shm_addr) { /* operation failed. */
      perror("consumer: Error: failed to attach the shared memeory");
      exit(EXIT_FAILURE);
  }
}
