#include "OtpLib.h"
#include <string>

using namespace std;

options loadArgs(int argc, char** argv) {
    options optStruct;
    const char* optTemp = "i:o:x:a:c:m:";
    int opt;

    while((opt = getopt(argc, argv, optTemp)) != -1){
        cout << "option: " << (char)opt 
            << ", arg: " << optarg
            << ", index: "<< optind
            << endl;

        switch (opt){
        case 'i':
            optStruct.inFilePath = optarg;
            break;
        case 'o':
            optStruct.outFilePath = optarg;
            break;
        case 'x':
            optStruct.seedData.x0 = atoi(optarg);
            cout << "X0: " << optStruct.seedData.x0 << endl;
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
    //Workers thread for calculating Vernam's encode
    auto context = (workersContext*)workerContext;
    string result = "";

    for (size_t i = context->startIndex;; i++){
        if(context->input[i] == NULL)
            break;

        char c = context->input[i] ^ (char)context->prngPtr[i];
        result += c;
    }
    
    cout << "Thread id: " << pthread_self() << ", result:= " << result << endl;

    pthread_barrier_wait(context->barrier);
    return reinterpret_cast<void*>(&result);
}


void workersBuffer::closeBuffer() {
    if(this->inputFileBuffer != nullptr) delete[] this->inputFileBuffer;

    //if fd is open close it!!!
}


bool workersBuffer::readFile(int fd) {
    this->inputFd = fd;
    this->readingFileSize = lseek(this->inputFd, 0, SEEK_END);
    lseek(this->inputFd, 0, 0); // Reset pointer to start
    this->inputFileBuffer = new char[this->readingFileSize];
    size_t fdReadRes = read(this->inputFd, (void*)this->inputFileBuffer, this->readingFileSize);

    if(fdReadRes == -1 || fdReadRes != this->readingFileSize){
        cerr << "Read result: " << fdReadRes << ", file size: "<< this->readingFileSize << endl;
        cerr << "Couldn't read the input file stoping programm" << endl;

        this->closeBuffer();
        close(this->inputFd);
        return false;
    }

    return true;
}


bool workersBuffer::writeFile(int fd) {
    this->outputFd = fd;

    ssize_t writeRes = write(this->outputFd, reinterpret_cast<void*>(&this->outputFileBuffer), this->readingFileSize);
    close(this->outputFd);

    return writeRes == this->readingFileSize;
}


void* generatePRNG(void* context) {
    auto con = (PRNGInfo*)context;
    if(con->prng == nullptr) con->prng = new size_t[con->rngLength];
    int x0 = con->_seed->x0;

    for (size_t i = 0; i < con->rngLength; i++){
        con->prng[i] = x0;
        x0 = (con->_seed->a * x0 + con->_seed->c)%con->_seed->m;
    }

    return nullptr;
}

