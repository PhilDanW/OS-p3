//Philip Wright
//CMP4760 Project3
//this part of the program is the main function that accepts command line arguments
//and feeds them into the monitor process to begin the main functionality of the program
#include <iostream>
#include <string.h>
#include <vector>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "libmonitor.h"

// Monitor always take 1, so we set to 19
const int MaxNumberOfChildren = 19;
const int MaxNumberOfSeconds = 100;

// declaration of usage function
static void show_usage(std::string);

using namespace std;

// main function that expects command line arguments
int main(int argc, char* argv[])
{
    // switch loop for argument processing
    int opt;
    //these values are entered as defaults if the userr does not specify any options
    string strLogFile = "logfile";
    int nNumberOfProducers = 2; 
    int nNumberOfConsumers = 6; 
    int nNumberOfSeconds = 100; 

    //switch through each parameter given
    opterr = 0;
    while ((opt = getopt(argc, argv, "ho:p:c:t:")) != -1) {
        switch (opt) {
            case 'h':
                show_usage(argv[0]);
                return EXIT_SUCCESS;
            case 'o':
                strLogFile = optarg;
                break;
            case 'p':
                nNumberOfProducers = atoi(optarg);
                break;
            case 'c':
                nNumberOfConsumers = atoi(optarg);
                break;
            case 't':
                nNumberOfSeconds = atoi(optarg);
                break;
            case '?':               
                if (isprint (optopt))
                {
                    errno = EINVAL;
                    perror("Unknown option");
                }
                else
                {
                    errno = EINVAL;
                    perror("Unknown option character");
                }
                return EXIT_FAILURE;
            default:
                perror ("master: Error: Illegal option found");
                show_usage(argv[0]);
                return EXIT_FAILURE;
        }
    }

    //now that we have input set the values if the user enter in specific values
    nNumberOfConsumers = min(nNumberOfConsumers, MaxNumberOfChildren-nNumberOfProducers);
    nNumberOfSeconds = min(nNumberOfSeconds, MaxNumberOfSeconds);

    if(nNumberOfConsumers < nNumberOfProducers)
    {
        perror ("master: Error: You must have more Consumers than Producers");
        show_usage(argv[0]);
        return EXIT_FAILURE;
    }

    // Output what is going to happen
    cout << "Monitor has started: " << endl 
        << "\t" << nNumberOfProducers << " is the number Producers" << endl
        << "\t" << nNumberOfConsumers << " is the number Consumers" << endl
        << "\t" << nNumberOfSeconds  << " is the number of Seconds" << endl << endl;

    // Start the monitor process, returning whatever monitor returns.
    return monitorProcess(strLogFile, nNumberOfProducers, nNumberOfConsumers, nNumberOfSeconds);

}


// Handle errors in input arguments by showing usage screen
static void show_usage(std::string name)
{
    std::cerr << std::endl
              << "Usage:\t" << name << " [-h]" << std::endl
              << "\t" << name << " [-h] [-o logfile] [-p m] [-c n] [-t time]" << std::endl
              << "Options:" << std::endl
              << "  -o logfile Name of the file to save logs" << std::endl
              << "  -p m Number of producers" << std::endl
              << "  -c n Number of consumers" << std::endl
              << "  -t time The time in seconds after which the process will terminate"<< std::endl 
              << std::endl;
}
