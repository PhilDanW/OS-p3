#include <iostream>
#include <string.h>
#include <vector>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "process.h"
using namespace std;

// Constants
const int children = 20;
const int seconds = 100;

// Main - expecting arguments
int main(int argc, char* argv[])
{
    string logFile = "logfile";
    int option;
    int producers = 2
    int consumers = 6;
    int timeSeconds = 100;

    // Go through each parameter entered and
    // prepare for processing
    opterr = 0;
    while ((option = getopt(argc, argv, "ho:p:c:t:")) != -1) {
        switch (option) {
            case 'h':
                prog_use(argv[0]);
                return EXIT_SUCCESS;
            case 'o':
                logFile = optarg;
                break;
            case 'p':
                producers = atoi(optarg);
                break;
            case 'c':
                consumers = atoi(optarg);
                break;
            case 't':
                timeSeconds = atoi(optarg);
                break;
            default:
                perror ("master: Error: Illegal option used on for the command. Check usage.");
                prog_use(argv[0]);
                return EXIT_FAILURE;
        }
    }

    consumers = min(consumers, children-producers);
    timeSeconds = min(timeSeconds, seconds);
    
    if(strLogFile.size() < 1) {
        errno = EINVAL;
        perror("master: Error: logfile does not exist ");
        return EXIT_FAILURE;
    }
    else if(nNumberOfProducers < 1) {
        errno = EINVAL;
        perror("master: Error: not enough producers given");
        return EXIT_FAILURE;
    }
    else if (nMaxNumberOfConsumers < 1) {
        errno = EINVAL;
        perror("master: Error: not enough consumers given");
        return EXIT_FAILURE;
    }
    else if(nSecondsToTerminate < 1) {
        errno = EINVAL;
        perror("master: Error: insufficient amount of time");
        return EXIT_FAILURE;
    }
  
    cout << producers << " is the number of Producers" << endl
    cout << consumers << " is the number of Consumers" << endl
    cout << timeSeconds << "is the number of Seconds" << endl;

    // Start the monitor process, returning whatever monitor returns.
    return process(logFile, producers, consumers, timeSeconds);

}
