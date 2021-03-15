#include <iostream>
#include <string.h>
#include <unistd.h>
#include <fstream>
#include <vector>
#include "semaphores.h"
#include "libmonitor.h"
#include "sharedstuff.h"
using namespace std;

struct itemPointer* product;
struct itemInfo* productQueue;

static int processes = 19;
static int buffer = 5120;

vector<int> vecProducers;
vector<int> vecConsumers;

bool WriteLogFile(std::string&, std::string);
std::string GetTimeFormatted(const char*);

volatile sig_atomic_t gSignalStatus = 0;
void signal_handler(int signal)
{
    gSignalStatus = 1;
}

//this is the main process to start the monitor
//during this process producers and consumers will be created as they are needed
int monitor(string strLogFile, int producers, int consumers, int seconds) {
  
  //register the signal handler
  signal(SIGINT, signal_handler);
  //create all variables needed
  
  bool isDead = false;
  bool isComplete = false;
  time_t elapSeconds;  
  elapSeconds = time(NULL);
  int consumerCount = 0;  
    
  const char* myLog = "monitor_log";
    
  cout << "some variables set" << endl;  
  
  string logstr = "Monitor process has begun...\n";
  WriteLogFile(logstr, strLogFile);
    
  // Create the Semaphores
  semaphores s(MUTEX, true, 1);
  semaphores n(EMPTY, true, 0);
  semaphores e(FULL, true, QUEUE_SIZE);
  
    
  cout << "made it to memory allocation" << endl;
  int memory = sizeof(itemPointer) + sizeof(itemInfo) * 20;
  cout << memory << endl;
  shm_id = shmget(SHARED, memory, IPC_CREAT | IPC_EXCL | 0660);
  cout << shm_id << endl;
  if (shm_id == -1) {
        perror("monitor: Error: could not allocate a segment of shared memory");
        exit(EXIT_FAILURE);
  }
  
  //if the memory segement was properly allocated, attach the segment to the process's address
  shm_addr = (char*)shmat(shm_id, NULL, 0);
  if (!shm_addr) { /* operation failed. */
        perror("monitor: Error: could not attach segment to process address");
        exit(EXIT_FAILURE);
  }
  
  // Get the queue header and the queue of products
  product = (struct itemPointer*) (shm_addr);
  product->currentItem = 0;
  product->nextItem = 0;
  product->size = QUEUE_SIZE;
  productQueue = (struct itemInfo*) (shm_addr+sizeof(int)+sizeof(product));

  
  for(int i=0; i < QUEUE_SIZE; i++)
  {
      productQueue[i].value = 0.0f;
      productQueue[i].ready = false;  
  }
  
  logstr = "Beginning process with the producers";
  WriteLogFile(logstr, strLogFile);
  
  // Start up producers by fork/exec nNumberOfProducers
  for(int i=0; i < producers; i++)
  {
    // Fork and store pid Producer Vector
    pid_t pid = fork(producerProg, myLog);
    if(pid > 0)
    {
      vecProducers.push_back(pid);
    }
  }
 
  cout << "monitor: Process has started with " << vecProducers.size() << " Producers" << endl << endl;

  if(vecProducers.size() < 1)
  {
    errno = ECANCELED;
    perror("monitor: Error: failed to create the necessary producers.");
    isDead = true;
  }
    
  pid_t waitPID;
  int waitStatus;
  
  cout << "outside the while loop" << endl;
  do
  {
    s.Wait();

    // Check for a waiting, readyToProcess queue
    if(productQueue[product->currentItem % QUEUE_SIZE].ready && vecConsumers.size() < (consumers+1))
    {
      // For a new consumer
      cout << "monitor: Assigning " << product->currentItem % QUEUE_SIZE << " to new consumer" << endl;
      pid_t pid = fork(consumerProg, strLogFile, product->currentItem%QUEUE_SIZE);
      if(pid > 0)
      {
          vecConsumers.push_back(pid);
          // Increment Current Index and wrap it around if > queue size
          product->currentItem = (++product->currentItem)%QUEUE_SIZE;
        
          // Report what happened ** Move Cursor left: \033[3D
          cout << "LibMonitor: Consumer PID " << pid << " started" << endl;
      }
    }

    s.Signal();
    
    // Note :: We use the WNOHANG to call waitpid without blocking
    // If it returns 0, it does not have a PID waiting
    waitPID = waitpid(-1, &waitStatus, WNOHANG | WUNTRACED | WCONTINUED);


    // No PIDs are in-process
    if (isDead) {
      isComplete = true;   // We say true so that we exit out of main
      break;              // loop and free up all necessary data
    }

    // Child processed correctly
    if (WIFEXITED(waitStatus) && waitPID > 0)
    {
      // Remove the consumer from the consumer array
      for(int i=0; i < vecConsumers.size(); i++)
      {
        if(vecConsumers[i] == waitPID)
        {
          cout << endl; // Put in a hard return.  Seems to look good
          vecConsumers.erase( vecConsumers.begin() + i);
          break;
        }
      }
    
    } else if (WIFSIGNALED(waitStatus) && waitPID > 0) {
        cout << waitPID << " killed by signal " << WTERMSIG(waitStatus) << endl;
    } else if (WIFSTOPPED(waitStatus) && waitPID > 0) {
        cout << waitPID << " stopped by signal " << WTERMSIG(waitStatus) << endl;
    } else if (WIFCONTINUED(waitStatus) && waitPID > 0) {
        continue;
    }

  } 
  while(!isDead && !gSignalStatus && !((time(NULL)-elapSeconds) > seconds));
    
  // Shutdown all of the producers
  cout << "Time to shut down the producers" << endl;
  for(int i=0; i < vecProducers.size(); i++)
  {
    kill(vecProducers[i], SIGQUIT); 
    cout << vecProducers[i] << "has been signaled to shutdown" << endl;
  }

  // Shutdown all of the consumers
  cout << "Time to shut down the consumers" << endl;
  for(int i=0; i < vecConsumers.size(); i++)
  {
    kill(vecConsumers[i], SIGQUIT); 
    cout << vecConsumers[i] << "has been signaled to shutdown" << endl;
  }
    
  // After all producers and consumers are shutdown, detatch and deallocate the shared memory
  cout << "Time to deallocate the shared memory" << endl;
  if (shmdt(shm_addr) == -1) {
      perror("monitor: Error: could not detach the shared memory");
  }
  else if (shmctl(shm_id, IPC_RMID, NULL) == -1) {
      perror("monitor: Error: could not deallocate the shared memory ");
  }
  else {
      cout << "monitor: Shared memory De-allocated" << endl << endl;
  }
    
  logstr = "The producers and consumers have been shutdown and all memory deallocated";
  WriteLogFile(logstr, strLogFile);
   
  return EXIT_SUCCESS;
}
               
int fork(string process, string myLog, int arrayItem)
{
        pid_t pid = fork();
        //if no child is forked, then exit
        if(pid < 0)
        {
            perror("monitor: Error: failed to fork process");
            return EXIT_FAILURE;
        }
        //if a child if forked, use exec to give it a job to do
        if(pid == 0)
        {
            if(arrayItem < 0) {
              execl(process.c_str(), process.c_str(), myLog.c_str(), (char*)0);
            }
            else {
              // Convert int to a c_str to send to exec
              string arrItem = getString(arrayItem);
              execl(process.c_str(), process.c_str(), arrItem.c_str(), myLog.c_str(), (char*)0);
            }

            fflush(stdout);
            exit(EXIT_SUCCESS);
        }
        else
            //return the given PID
            return pid; 
}

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
     
               

