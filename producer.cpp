#include <iostream>
#include <unistd.h>
#include <fstream>
#include "sharedStructures.h"
#include "productSemaphores.h"

// declaration of the usage function
static void show_usage(std::string);

// establish the signal handler
volatile sig_atomic_t sigQuitFlag = 0;
void sigQuitHandler(int sig){ 
  sigQuitFlag = 1; 
}

using namespace std;

int main(int argc, char* argv[])
{
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);

  // Use nano-seconds instead of seconds
  srand((time_t)ts.tv_nsec);

  // Register the signal handler
  signal(SIGINT, sigQuitHandler);
  
  // ProducerID to write to log
  int childPid = getpid();
  
  // Check if the correct number of arguments have been given
  if(argc < 2)
  {
    perror("Producer: Incorrect argument found");
    exit(EXIT_FAILURE);
  }
  // And make sure the log file is there
  string strLogFile = argv[1];


  // write to the log that startup of the child has begun
  string strLog = "Producer: PID ";
  strLog.append(GetStringFromInt(childPid));
  strLog.append(" started");
  WriteLogFile(strLog, strLogFile);
  cout << strLog << endl;

  // create the necessary Semaphores
  productSemaphores s(KEY_MUTEX, false);
  productSemaphores n(KEY_EMPTY, false);
  productSemaphores e(KEY_FULL, false);

  if(!s.isInitialized() || !n.isInitialized() || !e.isInitialized())
  {
    perror("Producer: Could not successfully find Semaphores");
    exit(EXIT_FAILURE);
  }
    // Allocate the shared memory
    shm_id = shmget(KEY_SHMEM, 0, 0);
    if (shm_id == -1) {
        perror("Producer: Could not successfully find Shared Memory");
        exit(EXIT_FAILURE);
    }

    // Read the memory size
    struct shmid_ds shmid_ds;
    shmctl(shm_id, IPC_STAT, &shmid_ds);
    size_t realSize = shmid_ds.shm_segsz;

    // setup with shmget
    shm_id = shmget(KEY_SHMEM, realSize, 0);
    if (shm_id == -1) {
        perror("Producer: Could not successfully find Shared Memory");
        exit(EXIT_FAILURE);
    }

    // attach the shared memory segment
    shm_addr = (char*)shmat(shm_id, NULL, 0);
    if (!shm_addr) { /* operation failed. */
        perror("Producer: Could not successfully attach Shared Memory");
        exit(EXIT_FAILURE);
    }

    // Get the queue header
    struct ProductHeader* productHeader = 
        (struct ProductHeader*) (shm_addr);
    // Get our queue right after the header
    struct ProductItem*productItemQueue = 
        (struct ProductItem*) (shm_addr+sizeof(int)+sizeof(productHeader));

  // Loop until signaled to shutdown via SIGINT
  while(!sigQuitFlag)
  {
    // Get a random time to sleep
    int nSleepTime = rand()%5+1;

    // Sleep for the random time
    sleep(nSleepTime);
    
    float myValue = 355.0f/113.0f;

    // Get Exclusive Access via Semaphores
    e.Wait();
    s.Wait();

    // Push this onto the Queue
    productItemQueue[productHeader->pNextQueueItem].itemValue = myValue;

    // Mark as ready to be process
    productItemQueue[productHeader->pNextQueueItem].readyToProcess = true;

    // Log what happened into the Log
    string strLog = "Producer: PID ";
    strLog.append(GetStringFromInt(childPid));
    strLog.append(" added item to queue: ");
    strLog.append(GetStringFromInt(productHeader->pNextQueueItem));
    WriteLogFile(strLog, strLogFile);
    cout << strLog << endl;

    // Add an item to the next queue and wrap it around if it's > queue size
    productHeader->pNextQueueItem = (++productHeader->pNextQueueItem)%productHeader->QueueSize;

    s.Signal();
    n.Signal();
  }

    return 0;
}
