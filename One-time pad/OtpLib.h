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

struct workersBuffer{
    int inputFd,
        outputFd;
    off_t readingFileSize;
    char* inputFileBuffer = nullptr;
    std::string outputFileBuffer = "";

    void closeBuffer();
    bool readFile(int fd);
    bool writeFile(int fd);
};

struct workersContext{
    pthread_t threadId;
    pthread_barrier_t* barrier;
    size_t* prngPtr;
    char* input;
    size_t startIndex; 
};

struct PRNGInfo{
    seed* _seed;
    size_t rngLength;
    size_t* prng = nullptr;
};

options loadArgs(int argc, char** argv);
void* encode(void* workerContext);
void* generatePRNG(void* context);

#endif // 


