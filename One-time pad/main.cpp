#include "OtpLib.h"
#include <pthread.h>

//TODO structurise this project
//TODO also replace 'iostream' with 'C' lib for formated output

int main(int argc, char *argv[]){
    int numCPU = sysconf(_SC_NPROCESSORS_ONLN);
    workersBuffer wBuffer;

    options globalOptions = loadArgs(argc, argv);

    //Make an another thread and use LCG algorithm to get PRN
    //dont forget to sync main and LCG thread!!!
    //use POSIX barriers for it

    int inFileFd = open(globalOptions.inFilePath, O_RDONLY);
    if(!wBuffer.readFile(inFileFd)) exit(EXIT_FAILURE);

    //workerBuffer struct for next instractions
    //create 'numCPU' threads
    //to parallel the Vernams encoding process 
    //while workers calculating new result main thread must be stoped!!!
    //each thread must return a result of its calculations
    //in main thread get all results. Sort and save them to output file

    std::cout << "Done!"<< std::endl;
    wBuffer.closeBuffer();
    return 0;
}
