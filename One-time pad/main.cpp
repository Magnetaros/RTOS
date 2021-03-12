#include "OtpLib.h"

//TODO structurise this project
//TODO also replace 'iostream' with 'C' lib for formated output

static pthread_barrier_t barrier;

int main(int argc, char* argv[]){
    int numCPU = sysconf(_SC_NPROCESSORS_ONLN);
    workersBuffer wBuffer;
    workersContext* wContexts = new workersContext[numCPU];

    options globalOptions = loadArgs(argc, argv);

    //Make an another thread and use LCG algorithm to get PRN
    //dont forget to sync main and LCG thread!!!
    //use POSIX barriers for it

    int inFileFd = open(globalOptions.inFilePath, O_RDONLY);
    if(!wBuffer.readFile(inFileFd)) exit(EXIT_FAILURE);

    pthread_t lcgThread;
    PRNGInfo prngInfo;
    prngInfo._seed = &globalOptions.seedData;
    prngInfo.rngLength = wBuffer.readingFileSize;

    int tCreateRes = pthread_create(&lcgThread, nullptr, generatePRNG, &prngInfo);

    if(tCreateRes == 0)
        pthread_join(lcgThread, nullptr);

    pthread_barrier_init(&barrier, nullptr, numCPU + 1);

    for (int i = 0; i < numCPU; i++){
        
        wContexts[i].barrier = &barrier;
        wContexts[i].input = wBuffer.inputFileBuffer;
        wContexts[i].prngPtr = prngInfo.prng;
        pthread_create(&wContexts[i].threadId, nullptr, encode, &wContexts[i]);
    }

    for(int i = 0; i < numCPU; i++){
        void* buffer;
        pthread_join(wContexts[i].threadId, &buffer);

        wBuffer.outputFileBuffer += *((std::string*)buffer);
    }
    
    pthread_barrier_wait(&barrier);

    int outFileFd = open(globalOptions.outFilePath, O_WRONLY);
    if(outFileFd != -1 && !wBuffer.writeFile(outFileFd)) 
        std::cout << "Error writting output file" << std::endl;

    
    // pthread_barrier_init();
    //use workerBuffer struct for next instractions
    //create 'numCPU' threads
    //to parallel the Vernams encoding process 
    //while workers calculating new result main thread must be stoped!!!
    //each thread must return a result of its calculations
    //in main thread get all results. Sort and save them to output file

    std::cout << "Done!"<< std::endl;
    wBuffer.closeBuffer();
    delete[] prngInfo.prng;
    return 0;
}
