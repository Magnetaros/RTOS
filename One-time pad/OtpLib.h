#ifndef OTPLIB_H
#define OTPLIB_H

#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>


struct seed{
    int x0;
    int a;
    int c;
    int m;
};

struct options{
    char* inFilePath;
    char* outFilePath;
    seed seedData;
};

struct workersContext{
    pthread_t threadId;
    pthread_barrier_t* barrier;
    int* prngPtr;
    char* input;
    std::string res;
    size_t startIndex; 
    size_t endIndex;
};

struct PRNGInfo{
    seed* _seed;
    size_t rngLength;
    int* prng = nullptr;
};

options loadArgs(int argc, char** argv);
void* encode(void* workerContext);
void* generatePRNG(void* context);
off_t readFd(int fd, char* &inputBuffer);
bool writeFd(int fd, char* &outputBuffer, size_t numByteToWrite);

#endif // 


