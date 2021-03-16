//Philip Wright
//CMP4760 - Project3
//this part of the program is the main driver function and is responsible for creating the 
//consumers and producers as well as semaphores
#include <iostream>
#include <string.h>
#include <vector>
#include <unistd.h>

#include "libmonitor.h"
#include "sharedStructures.h"
#include "productSemaphores.h"

//process counter
const int MAX_PROCESSES = 19;
const int BUFFERSIZE = 8192;

using namespace std;

// structures for going through the queue
struct ProductHeader* productHeader;
struct ProductItem* productItemQueue;
vector<int> vecProducers;
vector<int> vecConsumers;

// establish the signal handler
volatile sig_atomic_t sigIntFlag = 0;
void sigintHandler(int sig){ // can be called asynchronously
  sigIntFlag = 1; // set flag
}


//main process that starts the producers and consumers for the rest of the program
int monitorProcess(string strLogFile, int nNumberOfProducers, int nMaxNumberOfConsumers, int nSecondsToTerminate)
{
  // make sure all parameters meet the requirements to move forward
  if(strLogFile.size() < 1 || nNumberOfProducers < 1 || 
    nMaxNumberOfConsumers < 1 || nSecondsToTerminate < 1)
  {
    errno = EINVAL;
    perror("monitor process: could not recognize that option");
    return EXIT_FAILURE;
  }

  // Register the signal handler
  signal(SIGINT, sigintHandler);
  bool isKilled = false;
  bool bComplete = false;

  // start time 
  time_t secondsStart;
  secondsStart = time(NULL);

  // count the number of consumers that are active at one  time
  int nConsumerCount = 0;

  // write to the log
  string strLog = "Starting the monitor process...";
  WriteLogFile(strLog, strLogFile);


  // create the semaphores needed
  productSemaphores s(KEY_MUTEX, true, 1);
  productSemaphores n(KEY_EMPTY, true, 0);
  productSemaphores e(KEY_FULL, true, PRODUCT_QUEUE_LENGTH);

  if(!s.isInitialized() || !n.isInitialized() || !e.isInitialized())
  {
    perror("LibMonitor: Could not successfully create Semaphores");
    exit(EXIT_FAILURE);
  }
  
  // Setup shared memory
  int memSize = sizeof(ProductHeader) + sizeof(ProductItem) * PRODUCT_QUEUE_LENGTH;
  shm_id = shmget(KEY_SHMEM, memSize, IPC_CREAT | IPC_EXCL | 0660);
  if (shm_id == -1) {
      perror("monitor process: Failed to allocate shared memory");
      exit(EXIT_FAILURE);
  }
  // attach the shared memory segment
  shm_addr = (char*)shmat(shm_id, NULL, 0);
  if (!shm_addr) { /* operation failed. */
      perror("monitor process: Failed to attach shared memory");
      exit(EXIT_FAILURE);
  }
  // Get the queue header
  productHeader = (struct ProductHeader*) (shm_addr);
  // Get our entire queue
  productItemQueue = (struct ProductItem*) (shm_addr+sizeof(int)+sizeof(productHeader));

  // Fill product header
  productHeader->pCurrent = 0;
  productHeader->pNextQueueItem = 0;
  productHeader->QueueSize = PRODUCT_QUEUE_LENGTH;

  // Set all items in queue to empty
  for(int i=0; i < PRODUCT_QUEUE_LENGTH; i++)
  {
      productItemQueue[i].readyToProcess = false;
      productItemQueue[i].itemValue = 0.0f;
  }

  strLog = "monitor process: Starting up the producers";
  WriteLogFile(strLog, strLogFile);
  
  // use fork and exec to start up the producers
  for(int i=0; i < nNumberOfProducers; i++)
  {
    // Fork and store pid Producer Vector
    pid_t pid = forkProcess(ProducerProcess, strLogFile);
    if(pid > 0)
    {
      vecProducers.push_back(pid);
    }
  }
  cout << "monitor process: Started with " << vecProducers.size() << " Producers" << endl << endl;

  // Check that we actually have some producers
  if(vecProducers.size() < 1)
  {
    errno = ECANCELED;
    perror("monitor process: Failed to create Producers");
    isKilled = true;
  }
  
  // Keep track of waits & pids
  pid_t waitPID;
  int wstatus;

  // Loop until timeout or interrupt exit
  while(!isKilled && !sigIntFlag && !((time(NULL)-secondsStart) > nSecondsToTerminate))
  {
    // Check for newly available products
    s.Wait();


    // Check for a waiting, readyToProcess queue
    if(productItemQueue[productHeader->pCurrent%PRODUCT_QUEUE_LENGTH].readyToProcess &&
      vecConsumers.size() < (nMaxNumberOfConsumers+1))
    {
      //new consumer
      cout << "monitor process: Assigning " << productHeader->pCurrent%PRODUCT_QUEUE_LENGTH << " to new consumer" << endl;
      pid_t pid = forkProcess(ConsumerProcess, strLogFile, productHeader->pCurrent%PRODUCT_QUEUE_LENGTH);
      if(pid > 0)
      {
        // Keep track of the new consumer
        vecConsumers.push_back(pid);

        productHeader->pCurrent = (++productHeader->pCurrent)%PRODUCT_QUEUE_LENGTH;

        cout << "monitor process: Consumer PID " << pid << " has started" << endl;
      }
    }

    s.Signal();
    
    waitPID = waitpid(-1, &wstatus, WNOHANG | WUNTRACED | WCONTINUED);

    // No PIDs are in-process
    if (isKilled) {
      bComplete = true;   // We say true so that we exit out of main
      break;              
    }

    // Child processed correctly
    if (WIFEXITED(wstatus) && waitPID > 0)
    {
      // Remove the consumer
      for(int i=0; i < vecConsumers.size(); i++)
      {
        if(vecConsumers[i] == waitPID)
        {
          cout << endl;
          vecConsumers.erase( vecConsumers.begin() + i );
          break;
        }
      }
    
    } else if (WIFSIGNALED(wstatus) && waitPID > 0) {
        cout << waitPID << " killed by signal " << WTERMSIG(wstatus) << endl;
    } else if (WIFSTOPPED(wstatus) && waitPID > 0) {
        cout << waitPID << " stopped by signal " << WTERMSIG(wstatus) << endl;
    } else if (WIFCONTINUED(wstatus) && waitPID > 0) {
        continue;
    }

  }

  // Signal to the producers to shutdown
  cout << "monitor process: Shutting down producers" << endl;
  for(int i=0; i < vecProducers.size(); i++)
  {
    kill(vecProducers[i], SIGQUIT); 
    cout << "monitor process: Producer PID " << vecProducers[i] << " signaled shutdown" << endl;
  }

  cout << "monitor process: Shutting down consumers" << endl;
  for(int i=0; i < vecConsumers.size(); i++)
  {
    kill(vecConsumers[i], SIGQUIT); 
    cout << "monitor process: Consumer PID " << vecConsumers[i] << " signaled shutdown" << endl;
  }

  // Check for timeout
  if(sigIntFlag)
  {
    string strLog = "monitor process: Ctrl-C shutdown successful";
    WriteLogFile(strLog, strLogFile);
    cout << strLog << endl;
  }
  else
  {
    string strLog = "monitor process: Timeout shutdown successful";
    WriteLogFile(strLog, strLogFile);
    cout << strLog << endl;
  }

  // Dettach shared memory segment from process's address space
  cout << endl;
  cout << "monitor process: De-allocating shared memory" << endl;
  if (shmdt(shm_addr) == -1) {
      perror("monitor process: Error detaching shared memory");
  }

  // De-allocate the shared memory segment.
  if (shmctl(shm_id, IPC_RMID, NULL) == -1) {
      perror("monitor process: Error deallocating shared memory ");
  }

  cout << "monitor process: Shared memory De-allocated" << endl << endl;

  if(isKilled)
    return EXIT_FAILURE;

  strLog = "monitor process: : All producers and consumers terminated";
  WriteLogFile(strLog, strLogFile);

  return EXIT_SUCCESS;
}


int forkProcess(string strProcess, string strLogFile, int nArrayItem)
{
        pid_t pid = fork();
        // No child made - exit with failure
        if(pid < 0)
        {
            // Signal to any child process to exit

            perror("monitor process: Could not fork process");
            return EXIT_FAILURE;
        }
        // Child process here
        if(pid == 0)
        {
            // Execute child process without array arguements
            if(nArrayItem < 0)
              execl(strProcess.c_str(), strProcess.c_str(), strLogFile.c_str(), (char*)0);
            else
            {
              string strArrayItem = GetStringFromInt(nArrayItem);
              execl(strProcess.c_str(), strProcess.c_str(), strArrayItem.c_str(), strLogFile.c_str(), (char*)0);
            }

            fflush(stdout);
            // Exit from forked process successfully
            exit(EXIT_SUCCESS);    
        }
        else
            return pid; // Return the Parent PID
}
