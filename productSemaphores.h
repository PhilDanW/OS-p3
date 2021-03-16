//Philip Wright
//CMP4760 Project3
//this part of the program is just the .h file for productSemaphores.cpp
#ifndef PRODUCTSEMAPHORES
#define PRODUCTSEMAPHORES

#include <sys/sem.h>


class productSemaphores
{
    private:
    
        bool _bCreator;
        int _semid;
        bool _isInitialized;
        struct sembuf structSemaBuf;

    public:

    productSemaphores(key_t, bool, int = 1);
    ~productSemaphores();

    // Check if properly setup
    bool isInitialized() { return _isInitialized; };

    // Semaphore Wait
    void Wait();

    // Semaphore Signal
    void Signal();    

};

#endif
