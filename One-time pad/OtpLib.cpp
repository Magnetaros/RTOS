#include "OtpLib.h"
#include <string.h>

using namespace std;

Options loadArgs(int argc, char** argv) {
    Options optStruct;
    const char* optTemp = "i:o:x:a:c:m:";
    int opt;

    while((opt = getopt(argc, argv, optTemp)) != -1){
        switch (opt){
        case 'i':
            optStruct.inFilePath = optarg;
            break;
        case 'o':
            optStruct.outFilePath = optarg;
            break;
        case 'x':
            optStruct.seedData.x0 = atoi(optarg);
            break;
        case 'a':
            optStruct.seedData.a = atoi(optarg);
            break;
        case 'c':
            optStruct.seedData.c = atoi(optarg);
            break;
        case 'm':
            optStruct.seedData.m = atoi(optarg);
            break;
        case '?':
            cerr << "option " << argv[optind] << " need an argument!!" << endl;
            break;
        default:
            cerr << "wrong flag given!" << endl;
            cout << "Usage: -i inFilepath -o outFilepath -x0 num -a num -c num -m num" << endl;
            break;
        }
    }

    return optStruct;
}


void* encode(void* workerContext) {
    auto context = (WorkersContext*)workerContext;
    char* textToWorkWith;
    char* prng = reinterpret_cast<char*>(context->prngPtr);
    size_t bufferSize = context->endIndex - context->startIndex;
    context->res = new char[bufferSize];

    for (size_t i = context->startIndex, count = 0; i < context->endIndex; i++, count++){
        context->res[count] = (context->input[i] ^ prng[i]);
    }
    
    pthread_barrier_wait(context->barrier);
    return nullptr;
}

off_t readFd(int fd, char* &inputBuffer) {
    struct stat fStat;
    fstat(fd, &fStat);

    if(inputBuffer == nullptr)
        inputBuffer = new char[fStat.st_size];

    size_t bytesToRead = fStat.st_size;
    do{
        size_t fdReadRes = read(fd, inputBuffer, bytesToRead);

        if(fdReadRes == -1){
            cerr << "Read result: " << fdReadRes << ", file size: "<< fStat.st_size << endl;
            cerr << "Couldn't read the input file stoping programm" << endl;

            delete inputBuffer;
            close(fd);
            exit(EXIT_FAILURE);
        }

        bytesToRead -= fdReadRes;
    }while(bytesToRead != 0);

    return fStat.st_size;
}


bool writeFd(int fd, char* &outputBuffer, size_t numByteToWrite) {
    size_t bytesToWrite = numByteToWrite;

    do{
        ssize_t writeRes = write(fd, outputBuffer, bytesToWrite);
        if(writeRes == -1){
            cerr << "couldn't write in output file" << endl;
            close(fd);
            return false;
        }
        bytesToWrite -= writeRes;
    } while (bytesToWrite != 0);

    return true;
}


void* generatePRNG(void* context) {
    auto con = (PRNGInfo*)context;
    int* prng = new int[con->rngLength];
    int x0 = con->seed->x0;

    for (size_t i = 0; i < con->rngLength; i++){
        prng[i] = x0;
        x0 = (con->seed->a * x0 + con->seed->c)%con->seed->m;
    }

    return prng;
}
