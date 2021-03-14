#include <sys/sem.h>
#include <sys/stat.h>
#include <iostream>
#include "semaphores.h"
using namespace std;

productSemaphores::productSemaphores(key_t key, bool Create, int Value)
{
    // If the key is valid
    if(key > 0)
    {
        // this means we are creating a new key
        if(Create)
        {
            //give semget all its Permissions
            _semid = semget(key, 1, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH | IPC_EXCL | IPC_CREAT);
            // If successful, set it's value to Value
            if (_semid > 0)
            {
                semctl(_semid, 0, SETVAL, Value);
                // make it the creator of the semaphore
                _bCreator = true;
                // show that initialization is successfully done
                _isInitialized = true;
            }
        }
        else
        {
            // find an already created Semaphore
            _semid = semget(key, 1, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
            _bCreator = false;
            if (_semid > 0)
            {
                // show that initialization is successfully done
                _isInitialized = true;
            }
        }
    }
}

//semaphore class destructor
productSemaphores::~productSemaphores()
{
    if(_bCreator && _isInitialized)
    {
        semctl(_semid, 0, IPC_RMID);
    }
}

//semaphore wait function
void productSemaphores::Wait()
{
    structSemaBuf.sem_num = 0;
    structSemaBuf.sem_op = -1;
    structSemaBuf.sem_flg = 0;
    semop(_semid, &structSemaBuf, 1);
//	cout << "wait: " << _semid << endl;
}

// Semaphore Signal function
void productSemaphores::Signal() 
{
    structSemaBuf.sem_num = 0;
    structSemaBuf.sem_op = 1;
    structSemaBuf.sem_flg = 0;
    semop(_semid, &structSemaBuf, 1);
//	cout << "signal: " << _semid << endl;
}


