#include "OtpLib.h"
#include <string>

using namespace std;

options loadArgs(int argc, char** argv) {
    options optStruct;
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
    auto context = (workersContext*)workerContext;
    char* prng = reinterpret_cast<char*>(context->prngPtr);
    size_t bufferSize = context->endIndex - context->startIndex;

    for (size_t i = context->startIndex; i < context->endIndex; i++){
        context->res += (context->input[i] ^ prng[i]);
    }
    
    pthread_barrier_wait(context->barrier);
    return nullptr;
}

off_t readFd(int fd, char* &inputBuffer) {
    struct stat _fstat;
    fstat(fd, &_fstat);

    if(inputBuffer == nullptr)
        inputBuffer = new char[_fstat.st_size];

    size_t bytesToRead = _fstat.st_size;
    do{
        size_t fdReadRes = read(fd, inputBuffer, bytesToRead);

        if(fdReadRes == -1){
            cerr << "Read result: " << fdReadRes << ", file size: "<< _fstat.st_size << endl;
            cerr << "Couldn't read the input file stoping programm" << endl;

            delete inputBuffer;
            close(fd);
            exit(EXIT_FAILURE);
        }

        bytesToRead -= fdReadRes;
    }while(bytesToRead != 0);

    return _fstat.st_size;
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
    close(fd);

    return true;
}


void* generatePRNG(void* context) {
    auto con = (PRNGInfo*)context;
    if(con->prng == nullptr) con->prng = new int[con->rngLength];
    int x0 = con->_seed->x0;

    for (size_t i = 0; i < con->rngLength; i++){
        con->prng[i] = x0;
        x0 = (con->_seed->a * x0 + con->_seed->c)%con->_seed->m;
    }

    return con->prng;
}
