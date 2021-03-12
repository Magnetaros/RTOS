#ifndef OTPLIB_H
#define OTPLIB_H

#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

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
    size_t counter;
    char* inputFileBuffer = nullptr;
    char* outputFileBuffer = nullptr;

    void closeBuffer();
    bool readFile(int fd);
};

options loadArgs(int argc, char** argv);
void threadFunc(void *args);

#endif // 


