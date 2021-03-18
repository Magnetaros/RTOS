#include "OtpLib.h"

static pthread_barrier_t barrier;
//! need to fix sigf error
//! not all data can be processed: need to split all file into chunks

int main(int argc, char* argv[]){
    int numCPU = sysconf(_SC_NPROCESSORS_ONLN);
    off_t rFileSize;
    char* inputText  = nullptr;
    char* outputText = nullptr;
    workersContext wContexts[numCPU];

    options globalOptions = loadArgs(argc, argv);

    int inFileFd = open(globalOptions.inFilePath, O_RDONLY);
    rFileSize = readFd(inFileFd, inputText);

    pthread_t lcgThread;
    PRNGInfo prngInfo;
    prngInfo._seed     = &globalOptions.seedData;
    prngInfo.rngLength = rFileSize;

    int tCreateRes = pthread_create(&lcgThread, nullptr, generatePRNG, &prngInfo);

    if(tCreateRes == 0)
        pthread_join(lcgThread, (void**)prngInfo.prng);

    pthread_barrier_init(&barrier, nullptr, numCPU + 1);

    size_t chunckSize = rFileSize / numCPU;
    size_t fileEndOffset = rFileSize - (chunckSize * numCPU);
    std::cout << "File size: " << rFileSize << ", max chunck size: " << chunckSize * numCPU << ", file offset: " << fileEndOffset << std::endl;

    for (size_t i = 0; i < numCPU; i++){
        wContexts[i].barrier    = &barrier;
        wContexts[i].input      = inputText;
        wContexts[i].prngPtr    = prngInfo.prng;
        wContexts[i].startIndex = (chunckSize * i);
        wContexts[i].endIndex   = (wContexts[i].startIndex + chunckSize);
        if(i + 1 == numCPU) wContexts[i].endIndex += fileEndOffset;

        pthread_create(&wContexts[i].threadId, nullptr, encode, &wContexts[i]);
    }

    pthread_barrier_wait(&barrier);

    std::string encodedText;
    for(int i = 0; i < numCPU; i++)
        encodedText += wContexts[i].res;
    outputText = const_cast<char*>(encodedText.c_str());

    int outFileFd = open(globalOptions.outFilePath, O_WRONLY);
    if(outFileFd == -1 || !writeFd(outFileFd, outputText, rFileSize))
        std::cerr << "Error writing encoded text to a file" << std::endl;

    std::cout << "Done!"<< std::endl;
    delete[] inputText;
    delete[] outputText;
    delete[] prngInfo.prng;
    return 0;
}
