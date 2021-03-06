//Philip Wright
//CMP4760 Project3
//this part of the program is a special class that is used to make semaphores 
//for the monitor process
#include <sys/sem.h>
#include <sys/stat.h>
#include <iostream>
#include "productSemaphores.h"

// Params for Semaphores
#define PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)

using namespace std;

productSemaphores::productSemaphores(key_t key, bool Create, int Value)
{
    // If a valid key
    if(key > 0)
    {
        // If Creating a new Key
        if(Create)
        {
//            #if defined(__linux__)
            _semid = semget(key, 1, PERMS | IPC_EXCL | IPC_CREAT);
//            #else
//            _semid = semget(key, 1, SEM_R | SEM_A | IPC_EXCL | IPC_CREAT);
//            #endif
            // If successful, set it's value to Value
            if (_semid > 0)
            {
                semctl(_semid, 0, SETVAL, Value);
                // Write success to log file

                // Set as the creator of the Sem
                _bCreator = true;
                // Set as properly initialized
                _isInitialized = true;
            }
        }
        else
        {
            // Get created Semaphore
            _semid = semget(key, 1, PERMS);
            _bCreator = false;
            if (_semid > 0)
            {
                // Set as properly initialized
                _isInitialized = true;
            }
        }
    }
}

productSemaphores::~productSemaphores()
{
    if(_bCreator && _isInitialized)
    {
        semctl(_semid, 0, IPC_RMID);

        // Log as released
    }
}

void productSemaphores::Wait()
{
    structSemaBuf.sem_num = 0;
    structSemaBuf.sem_op = -1;
    structSemaBuf.sem_flg = 0;
    semop(_semid, &structSemaBuf, 1);
//	cout << "wait: " << _semid << endl;
}

// Semaphore Signal
void productSemaphores::Signal() 
{
    structSemaBuf.sem_num = 0;
    structSemaBuf.sem_op = 1;
    structSemaBuf.sem_flg = 0;
    semop(_semid, &structSemaBuf, 1);
}
