#include <iostream>
#include <string.h>
#include <vector>
#include <unistd.h>
#include "process.h"
#include "stuff.h"
#include "productSemaphores.h"
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

int process(string myLog, int producers, int consumers, int seconds) {
  //register the signal handler
  signal(SIGINT, signal_handler);
  //start the timer;
  elapSeconds = time(NULL);
  
  log = "Monitor process has begun...\n";
  WriteLogFile(log, myLog);
  
  //setup shared memory and allocate a segment with length of the queue * (size of product + size of product queue)
  int memory = PRODUCT_QUEUE_LENGTH * (sizeof(ProductHeader) + sizeof(ProductItem));
  shm_id = shmget(KEY_SHMEM, memSize, IPC_CREAT | IPC_EXCL | 0660);
  if (shm_id == -1) {
      perror("process: Error: could not allocate a segment of shared memory");
      exit(EXIT_FAILURE);
  }
  
  //if the memory segement was properly allocated, attach the segment to the process's address
  shm_addr = (char*)shmat(shm_id, NULL, 0);
  if (!shm_addr) { /* operation failed. */
      perror("process: Error: could not attach segment to process address");
      exit(EXIT_FAILURE);
  }
  
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
    pid_t pid = forkProcess(producerProg, myLog);
    if(pid > 0)
    {
      producerArray[i] = pid;(pid);
    }
  }
  cout << "process: Process has started with " << (sizeof(producerArray) / sizeof(producerArray[0])) << " Producers" << endl << endl;

  if((sizeof(producerArray) / sizeof(producerArray[0])) < 1)
  {
    errno = ECANCELED;
    perror("process: Error: failed to create the necessary producers.");
    isDead = true;
  }
  
  
  
  
void produce_consume(bool isDead, int SigIntFlag, time_t elapsed, int seconds) {
  while(!isDead && !sigIntFlag && !((time(NULL)-elapsed) > seconds) {
    // Check for new products to consume
    s.Wait();
    
    // Check for a waiting, readyToProcess queue
    if(productQueue[product.currentItem % QUEUE_SIZE].ready)
    {
      // For a new consumer
      cout << "process: Assigning " << product.currentItem % QUEUE_SIZE << " to consumer" << endl;
      pid_t pid = fork(consumerProg, myLog, product.currentItem % QUEUE_SIZE);
      
      for(int i=0; i < consumers; i++) {
        if(pid > 0)
        {
          // Keep track of the new consumer in consumer vector
          consumerArray[i] = pid;

          // Increment Current Index and wrap it around if > queue size
          product.currentItem = (++product.currentItem) % QUEUE_SIZE;
        
          // Report what happened ** Move Cursor left: \033[3D
          cout << "process: the consumer pid " << pid << " started" << endl;
      }
    }
