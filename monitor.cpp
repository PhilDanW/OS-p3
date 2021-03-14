#include <iostream>
#include <string.h>
#include <vector>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "libmonitor.h"
using namespace std;

// Constants
const int numChildren = 20;
const int numSeconds = 100;

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

    consumers = min(consumers, numChildren-producers);
    timeSeconds = min(timeSeconds, numSeconds);
  
    cout << producers << " Producers" << endl
         << consumers << " Consumers" << endl
         << timeSeconds  << " Max Seconds" << endl;

    // Start the monitor process, returning whatever monitor returns.
    return monitorProcess(lFile, nNumberOfProducers, nNumberOfConsumers, nNumberOfSeconds);

}
