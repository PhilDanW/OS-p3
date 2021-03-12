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

// Forward declarations
static void prog_use(std::string);



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


// Handle errors in input arguments by showing usage screen
static void prog_use(std::string str)
{
    std::cerr << std::endl
              << "Use:\t" << str << " [-h]" << std::endl
              << "\t" << str << " [-h] [-o logfile] [-p m] [-c n] [-t time]" << std::endl
              << "Command Line Options:" << std::endl
              << "  -o logfile Name of the file to save logs" << std::endl
              << "  -p m Number of producers" << std::endl
              << "  -c n Number of consumers" << std::endl
              << "  -t the time in which the process will terminate"
              << std::endl << std::endl;
}
