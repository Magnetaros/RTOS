#include "OtpLib.h"

using namespace std;

void loadArgs(int argc, char** argv) {
    const char *optTemp = "i:o:x:a:c:m:";
    int opt;

    while((opt = getopt(argc, argv, optTemp)) != -1){

        cout << "option: " << (char)opt 
            << ", arg: " << optarg
            << ", index: "<< optind
            << endl;

        switch (opt){
        case 'i':
            globalOptions.inFilePath = optarg;
            break;
        case 'o':
            globalOptions.outFilePath = optarg;
            break;
        case 'x':
            globalOptions.seedData.x0 = atoi(optarg);
            break;
        case 'a':
            globalOptions.seedData.a = atoi(optarg);
            break;
        case 'c':
            globalOptions.seedData.c = atoi(optarg);
            break;
        case 'm':
            globalOptions.seedData.m = atoi(optarg);
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
}


void threadFunc(void *args) {
    //Workers thread for calculating Vernams encode
}


void workersBuffer::closeBuffer() {
    if(this->inputFileBuffer != nullptr) delete this->inputFileBuffer;
    if(this->outputFileBuffer != nullptr) delete this->outputFileBuffer;

    //if fd is open close!!!
}


bool workersBuffer::readFile(int fd) {
    this->inputFd = fd;
    this->readingFileSize = lseek(this->inputFd, 0, SEEK_END);
    this->inputFileBuffer = new char[this->readingFileSize];
    size_t fdReadRes = read(this->inputFd, (void*)this->inputFileBuffer, this->readingFileSize);

    cout << "Created buffer with size of "<< sizeof(this->inputFileBuffer) << " bytes" << endl;

    if(fdReadRes == -1 || fdReadRes != this->readingFileSize){
        cerr << "Read result: " << fdReadRes << ", file size: "<< this->readingFileSize << endl;
        cerr << "Couldn't read the input file stoping programm" << endl;

        this->closeBuffer();
        close(this->inputFd);
        return false;
    }

    return true;
}