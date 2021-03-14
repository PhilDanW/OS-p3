#include <iostream>
#include <string.h>
#include <vector>
#include <unistd.h>
#include "monitor.h"
#include "sharedstuff.h"
using namespace std;

struct itemPointer* product;
struct itemInfo* productQueue;

static int processes = 19;
static int buffer = 5120;
int consumerCount = 0;
time_t elapSeconds = NULL;
string log = NULL;
bool isDead = false;
bool isComplete = false;
pid_t wait;
int waitStatus;

int producerArray[100] = {0};
int consumerArray[100] = {0};

volatile sig_atomic_t gSignalStatus = 0;
void signal_handler(int signal)
{
    gSignalStatus = signal;
}

int monitor(string myLog, int producers, int consumers, int seconds) {
  //register the signal handler
  signal(SIGINT, signal_handler);
  //start the timer;
  elapSeconds = time(NULL);
  
  log = "Monitor process has begun...\n";
  WriteLogFile(log, myLog);
  
  allocateMemory();
  
  // Get the queue header and the queue of products
  product = (struct ProductHeader*) (shm_addr);
  product.currentItem = 0;
  product.nextItem = 0;
  product.size = QUEUE_SIZE;
  productQueue = (struct ProductItem*) (shm_addr+sizeof(int)+sizeof(productHeader));

  
  for(int i=0; i < QUEUE_SIZE; i++)
  {
      productQueue[i].value = 0.0f;
      productQueue[i].ready = false;  
  }
  
  log = "Beginning process with the producers";
  WriteLogFile(log, myLog);
  
  // Start up producers by fork/exec nNumberOfProducers
  for(int i=0; i < producers; i++)
  {
    // Fork and store pid Producer Vector
    pid_t pid = fork(producerProg, myLog);
    if(pid > 0)
    {
      producerArray[i] = pid;
    }
  }
  int prodArraySize = sizeof(producerArray) / sizeof(producerArray[0]);
  int consArraySize = sizeof(consumerArray) / sizeof(consumerArray[0]);
  cout << prodArraySize << endl;
  cout << consArraySize << endl;
  cout << "monitor: Process has started with " << prodArraySize << " Producers" << endl << endl;

  if(prodArraySize < 1)
  {
    errno = ECANCELED;
    perror("monitor: Error: failed to create the necessary producers.");
    isDead = true;
  }
  
  produce_consume(isDead, SigIntFlag, elapsed, seconds);
    
  // Shutdown all of the producers
  cout << "Time to shut down the producers" << endl;
  for(int i=0; i < prodArraySize; i++)
  {
    kill(producerArray[i], SIGQUIT); 
    cout << producerArray[i] << "has been signaled to shutdown" << endl;
  }

  // Shutdown all of the consumers
  cout << "Time to shut down the consumers" << endl;
  for(int i=0; i < consArraySize; i++)
  {
    kill(consumerArray[i], SIGQUIT); 
    cout << consumerArray[i] << "has been signaled to shutdown" << endl;
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
    
  log = "The producers and consumers have been shutdown and all memory deallocated";
  WriteLogFile(log, myLog);
    
  return EXIT_SUCCESS;
}
    
  

  
  
void produce_consume(bool isDead, int SigIntFlag, time_t elapsed, int seconds) {
  while(!isDead && !sigIntFlag && !((time(NULL)-elapsed) > seconds) 
  {
    // Check for new products to consume
    s.Wait();
    
    // Check for a waiting, readyToProcess queue
    if(productQueue[product.currentItem % QUEUE_SIZE].ready) 
    {
      // For a new consumer
      cout << "monitor: Assigning " << product.currentItem % QUEUE_SIZE << " to consumer" << endl;
      pid_t pid = fork(consumerProg, myLog, product.currentItem % QUEUE_SIZE);
      
      
        if(pid > 0) 
        {
          for(int i=0; i < consumers; i++) 
          {
              // Keep track of the new consumer in consumer vector
              consumerArray[i] = pid;

              // Increment Current Index and wrap it around if > queue size
              product.currentItem = (++product.currentItem) % QUEUE_SIZE;
           }
           // Report what happened ** Move Cursor left: \033[3D
           cout << "monitor: the consumer pid " << pid << " started" << endl;
         }
    }
        
    // waitpid() suspends execution of the current process until a child specified by pid argument has changed state  
    // WNOHANG returns immediately if no child has exited.
    // WUNTRACED returns if a child has stopped
    // WCONTINUED returns if a stopped child has been resumed
    wait = waitpid(-1, &waitStatus, WNOHANG | WUNTRACED | WCONTINUED);    
    
    //Check to see if no PIDs are in-process
    if (isDead) {
      isComplete = true; 
      break;              
    }

    // if the child process was done correctly
    if (WIFEXITED(waitStatus) && wait > 0) 
    {
        // take the consumer out of the consumer array
        for(int i=0; i < consArraySize ; i++) 
        {
            if(consumerArray[i] == waitPID 
            {
                consumerArray[i] = NULL;
                break;
            }
        }
     } 
     else if (wait && WIFSIGNALED(waitStatus) > 0) 
     {
        cout << "Killed by signal. PID: " << wait << WTERMSIG(waitStatus) << endl;
     } 
     else if (wait && WIFSIGNALED(waitStatus) > 0) 
     {
        cout << "Stopped by signal. PID: " << wait << WTERMSIG(waitStatus) << endl;
     } 
     else if (wait && WIFSIGNALED(waitStatus) > 0) 
     {
            continue;
     }
  }
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
              std::string arrayItem = std::to_string(ArrayItem);
              execl(process.c_str(), process.c_str(), arrayItem.c_str(), myLog.c_str(), (char*)0);
            }

            fflush(stdout);
            exit(EXIT_SUCCESS);
        }
        else
            //return the given PID
            return pid; 
}
               
//setup shared memory and allocate a segment with length of the queue * (size of product + size of product queue)
void allocateMemory() {
    int memory = QUEUE_SIZE * (sizeof(itemPointer) + sizeof(itemInfo));
    shm_id = shmget(KEY_SHMEM, memory, IPC_CREAT | IPC_EXCL | 0660);
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
}
     
               

