#ifndef OTPLIB_H
#define OTPLIB_H

#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>

struct Seed{
    int x0;
    int a;
    int c;
    int m;
};

struct Options{
    char* inFilePath;
    char* outFilePath;
    Seed seedData;
};

struct WorkersContext{
    pthread_t threadId;
    pthread_barrier_t* barrier;
    int* prngPtr;
    char* input;
    char* res;
    size_t startIndex; 
    size_t endIndex;
};

struct PRNGInfo{
    pthread_t threadId;
    Seed* seed;
    size_t rngLength;
    int* prng = nullptr;
};

Options loadArgs(int argc, char** argv);
void* encode(void* workerContext);
void* generatePRNG(void* context);
off_t readFd(int fd, char* &inputBuffer);
bool writeFd(int fd, char* &outputBuffer, size_t numByteToWrite);

#endif // 
