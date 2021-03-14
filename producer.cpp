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
  
    int Pid = getpid();
    std::string myPid = std::to_string(Pid)
    string log = "Producer's PID: ";
    log.append(myPid);
    log.append(" has started.");
    WriteToLog(log, myLog);
  
    productSemaphores s(KEY_MUTEX, false);
    productSemaphores n(KEY_EMPTY, false);
    productSemaphores e(KEY_FULL, false);
  
    allocateMemory();
  
    // Get the queue header
    struct itemPointer* head = (struct itemPointer*) (shm_addr);
    // Get our entire queue
    struct itemInfo* queue  = (struct itemInfo*) (shm_addr + sizeof(int) + sizeof(head));
  
    while(!sigQuitFlag)
    {
        int sleepTime = rand() % 5 + 1;
        sleep(sleepTime);
    
  
        e.Wait();
        s.Wait();
      
        int myValue = ((1 + 2) * (3 + 4) * (5 + 6));
      
        queue[head.nextItem].value = myValue;
        queue[head->nextItem].ready = true;

        // Log what happened into System Log
        std::string myPID = std::to_string(Pid); 
        std::string myItem = std::to_string(head.nextItem);
        log = "Producer's PID: ";
        log.append(myPID);
        log.append(" put item in queue: ");
        log.append(myItem);
        WriteLogFile(log, myLog);

        cout << log << endl;
      
        head.nextItem =(++head.nextItem) % head.size;

        s.Signal();
        n.Signal();

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
