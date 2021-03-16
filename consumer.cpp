//Philip Wright
//CMP4760 - Project3
//the part of the program controls the functions of the consumers and begins in the monitor process
#include <iostream>
#include <unistd.h>
#include "sharedStructures.h"
#include "productSemaphores.h"
#include <fstream>

//declare the usage function
static void show_usage(std::string);

//establish the signal handler
volatile sig_atomic_t sigQuitFlag = 0;
void sigQuitHandler(int sig){ // can be called asynchronously
  sigQuitFlag = 1; // set flag
}

using namespace std;

int main(int argc, char* argv[])
{
  // Get the Index to the array assigned to this consumer
  if(argc < 2)
  {
    cout << "Args: " << argv[0] << endl;
    perror("Consumer: invalid argument found");
    exit(EXIT_FAILURE);
  }
  // get the index of the item to process
  const int nItemToProcess = atoi(argv[1]);

  // get the log file string
  string strLogFile = argv[2];

  // Seed the randomizer
  srand(time(NULL));

  // Register the signal handler
  signal(SIGINT, sigQuitHandler);

  // Log startup of the child
  const pid_t nPid = getpid();
  string strLog = "The Consumer PID is: ";
  strLog.append(GetStringFromInt(nPid));
  strLog.append(" Started");
  WriteLogFile(strLog, strLogFile);

  // get the necessary Semaphores
  productSemaphores s(KEY_MUTEX, false);
  productSemaphores n(KEY_EMPTY, false);
  productSemaphores e(KEY_FULL, false);

  if(!s.isInitialized() || !n.isInitialized() || !e.isInitialized())
  {
    perror("consumer process: Could not find Semaphores");
    exit(EXIT_FAILURE);
  }

  // Allocate the shared memory
  shm_id = shmget(KEY_SHMEM, 0, 0);
  if (shm_id == -1) {
      perror("consumer process: Could not find Shared Memory");
      exit(EXIT_FAILURE);
  }

  // Read the memory size
  struct shmid_ds shmid_ds;
  shmctl(shm_id, IPC_STAT, &shmid_ds);
  size_t realSize = shmid_ds.shm_segsz;

  //setup with shmget
  shm_id = shmget(KEY_SHMEM, realSize, 0);
  if (shm_id == -1) {
      perror("consumer process: Could not find Shared Memory");
      exit(EXIT_FAILURE);
  }

  // attach the shared memory segment
  shm_addr = (char*)shmat(shm_id, NULL, 0);
  if (!shm_addr) { /* operation failed. */
      perror("consumer process: Could not attach Shared Memory");
      exit(EXIT_FAILURE);
  }

  // Get the queue header
  struct ProductHeader* productHeader = 
      (struct ProductHeader*) (shm_addr);
  // Get our entire queue
  struct ProductItem*productItemQueue = 
      (struct ProductItem*) (shm_addr+sizeof(int)+sizeof(productHeader));

  // Get a random time to sleep
  int nSleepTime = rand()%10+1;

  // Sleep for the time
  while(!sigQuitFlag && !nSleepTime)
  {
    sleep(nSleepTime);
    nSleepTime--;
  }
  
  n.Wait();
  s.Wait();

  // Consume the value
  float fNewPiVal =
    productItemQueue[nItemToProcess].itemValue;

  // Reset values
  productItemQueue[nItemToProcess].itemValue = 0.0f;
  productItemQueue[nItemToProcess].readyToProcess = false;

  // Log what happened into System Log
  strLog = "Consumer: PID ";
  strLog.append(GetStringFromInt(nPid));
  strLog.append(" Consumed Item in Queue: ");
  strLog.append(GetStringFromInt(nItemToProcess));
  WriteLogFile(strLog, strLogFile);

  cout << "Consumer PID is: " << nPid << " consumed item in queue: " << nItemToProcess << endl;

  s.Signal();
  e.Signal();

  // Just let Consumer successfully die after it's done
  return EXIT_SUCCESS;
}
