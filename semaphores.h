#ifndef SEMAPHORES
#define SEMAPHORES

#include <sys/sem.h>

class semaphores
{
    public:
        semaphores(key_t, bool, int = 1);
        ~semaphores();
        // Check if properly setup
        bool isInitialized() { return _isInitialized; };
        // Semaphore Wait
        void Wait();
        // Semaphore Signal
        void Signal();  
    
    private:
        bool _creator;
        int _semid;
        bool _isInitialized;
        struct sembuf structSemaBuf;
};

#endif
