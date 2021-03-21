#include "OtpLib.h"

int main(int argc, char* argv[]){
    pthread_barrier_t barrier;
    off_t rFileSize;
    char* inputText  = nullptr;
    char* outputText = nullptr;
    int numCPU = sysconf(_SC_NPROCESSORS_ONLN);
    WorkersContext wContexts[numCPU];

    Options globalOptions = loadArgs(argc, argv);

    int inFileFd = open(globalOptions.inFilePath, O_RDONLY);
    rFileSize = readFd(inFileFd, inputText);

    pthread_t lcgThread;
    void *prngResult;
    PRNGInfo prngInfo;
    prngInfo.seed      = &globalOptions.seedData;
    prngInfo.rngLength = rFileSize;

    int tCreateRes = pthread_create(&lcgThread, nullptr, generatePRNG, &prngInfo);

    if(tCreateRes == 0){
        pthread_join(lcgThread, &prngResult);
        prngInfo.prng = (int*)prngResult;
    }else{
        std::cerr << "Couldn't created a thread for LCG method" << std::endl;
        if(prngInfo.prng != nullptr) delete[] prngInfo.prng;
        return EXIT_FAILURE;
    }

    pthread_barrier_init(&barrier, nullptr, numCPU + 1);

    size_t chunckSize = rFileSize / numCPU;
    size_t EOFOffset = rFileSize - (chunckSize * numCPU);

    for (size_t i = 0; i < numCPU; i++){
        wContexts[i].barrier    = &barrier;
        wContexts[i].input      = inputText;
        wContexts[i].prngPtr    = prngInfo.prng;
        wContexts[i].startIndex = (chunckSize * i);
        wContexts[i].endIndex   = (wContexts[i].startIndex + chunckSize);
        if(i + 1 == numCPU) wContexts[i].endIndex += EOFOffset;

        pthread_create(&wContexts[i].threadId, nullptr, encode, &wContexts[i]);
    }

    pthread_barrier_wait(&barrier);
    pthread_barrier_destroy(&barrier);

    int outFileFd = open(globalOptions.outFilePath, O_WRONLY);
    for (size_t i = 0; i < numCPU; i++){
        size_t bytesToWrite = wContexts[i].endIndex - wContexts[i].startIndex;
        if(outFileFd == -1 || !writeFd(outFileFd, wContexts[i].res, bytesToWrite))
            std::cerr << "Error writing encoded text to a file" << std::endl;    
        delete[] wContexts[i].res;
    }
    close(outFileFd);

    std::cout << "Done!" << std::endl;
    delete[] inputText;
    delete[] outputText;
    delete[] prngInfo.prng;
    return 0;
}
