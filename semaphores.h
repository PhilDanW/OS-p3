#ifndef SEMAPHORES
#define SEMAPHORES

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
