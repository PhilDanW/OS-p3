#include <iostream>
#include <unistd.h>
#include "stuff.h"
#include <fstream>
using namespace std;



volatile sig_atomic_t gSignalStatus = 0;
void signal_handler(int signal)
{
    gSignalStatus = 1;
}

int main(int argc, char* argv[])
{
    int item = atoi(argv[1]);
    string myLog = argv[2];
    srand(time(NULL));
    int sleepTime = rand() % 10 + 1;
    signal(SIGINT, signal_handler);
  
    pid_t Pid = getpid();
  
    string log = "Consumer's PID: " << std::string arrayItem = std::to_string(Pid) << " has started."
    WriteToLog(log, myLog);
  
    productSemaphores s(KEY_MUTEX, false);
    productSemaphores n(KEY_EMPTY, false);
    productSemaphores e(KEY_FULL, false);
  
    allocateMemory();
  
    // Get the queue header
    struct itemPointer* head = (struct itemPointer*) (shm_addr);
    // Get our entire queue
    struct itemInfo* queue  = (struct itemInfo*) (shm_addr + sizeof(int) + sizeof(head));
  
    while(!sigQuitFlag && !sleepTime)
    {
      sleep(sleepTime);
      sleepTime--;
    }
  
    n.Wait();
    s.Wait();

    // Consume the value
    float fNewPiVal = queue[item].value;

    // Reset values
    queue[item].value = 0.0f;
    queue[item].ready = false;

    // Log what happened into System Log
    std::string myPID = std::to_string(Pid); 
    std::string myItem = std::to_string(item);
    log = "Consumer's PID: ";
    log.append(myPID);
    log.append(" Consumed Item in Queue: ");
    log.append(myItem);
    WriteLogFile(strLog, strLogFile);

    cout << "Consumer: " << nPid << " consumed item in queue: " << nItemToProcess << endl;

    s.Signal();
    e.Signal();

    return EXIT_SUCCESS;
}

void allocateMemory() {
  
  shm_id = shmget(KEY_SHMEM, 0, 0);
  if (shm_id == -1) {
      perror("consumer: Error: failed to find shm_id " << shm_id << endl);
      exit(EXIT_FAILURE);
  }

  // Get the size of the memory
  struct shmid_ds shmid_ds;
  shmctl(shm_id, IPC_STAT, &shmid_ds);
  size_t realSize = shmid_ds.shm_segsz;

  // use shmget to setup with the size of the memory
  shm_id = shmget(KEY_SHMEM, realSize, 0);
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
