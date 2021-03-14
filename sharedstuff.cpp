#include <sys/sem.h>
#include <sys/stat.h>
#include <iostream>
#include "sharedstuff.h"
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
            _semid = semget(key, 1, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH | IPC_EXCL | IPC_CREAT);
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
            // Get an already created Semaphore
//            #if defined(__linux__)
            _semid = semget(key, 1, PERMS);
//            #else
//            _semid = semget(key, 1, SEM_R | SEM_A);
//            #endif


//            _semid = semget(key, 1, SEM_R | SEM_A );
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
//	cout << "signal: " << _semid << endl;
}

bool WriteToLog(std::string& logString, std::string LogFile)
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
        perror("Failed to write to the log");
        return false;
    }
}

static void prog_use(std::string name)
{
   std::cerr << std::endl << "\t" << name << " [-h] [-o logfile] [-p m] [-c n] [-t time]" << std::endl
   << "Input options:" << std::endl
   << "-o 'logfile' Name of file to keep program logs" << std::endl
   << "-p 'x' number of producers to make" << std::endl
   << "-c 'y' number of consumers to make" << std::endl
   << "-t 'time' seconds after which the process will terminate"<< std::endl 
   << std::endl;
}
