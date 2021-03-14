#include <iostream>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "monitor.h"
#include "sharedstuff.h"
using namespace std;

// Constant values for the max number of children and seconds allowed
const int children = 20;
const int seconds = 100;

//gather command line arguments here
int main(int argc, char* argv[])
{
    string logFile = "This is the LogFile";
    int option;
    int producers = 2;
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

    //set the actual values for the number of consumers and producers for the program
    consumers = min(consumers, children-producers);
    timeSeconds = min(timeSeconds, seconds);
    
    //check if all values are valid
    if(logFile.size() < 1) {
        errno = EINVAL;
        perror("master: Error: the logfile does not exist ");
        return EXIT_FAILURE;
    }
    else if(producers < 1) {
        errno = EINVAL;
        perror("master: Error: not enough producers given");
        return EXIT_FAILURE;
    }
    else if (consumers < 1) {
        errno = EINVAL;
        perror("master: Error: not enough consumers given");
        return EXIT_FAILURE;
    }
    else if(timeSeconds < 1) {
        errno = EINVAL;
        perror("master: Error: insufficient amount of time");
        return EXIT_FAILURE;
    }
  
    // output the parameters that are going to be sent to the monitor process
    cout << producers << " is the number of Producers" << endl;
    cout << consumers << " is the number of Consumers" << endl;
    cout << timeSeconds << "is the number of Seconds" << endl;

    // Start the monitor process, returning whatever monitor returns.
    return monitor(logFile, producers, consumers, timeSeconds);

}
