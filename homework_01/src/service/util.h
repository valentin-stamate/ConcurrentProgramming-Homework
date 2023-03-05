#ifndef HOMEWORK_01_UTIL_H
#define HOMEWORK_01_UTIL_H


#include <netinet/in.h>

class Util {
public:
    static char* getIp();
    static long int getFileSize(char* fileName);
    static char** getFilesFromDirectory(char* path);
    static int getFileCount(char* path);
    static char* getFileNameFromPath(char* path);
    static void removeFiles(char* path);

    static void writeTo(int protocol, int fd, void* buffer, int bufferSize, sockaddr_in &addr);
    static void readFrom(int protocol, int fd, void* buffer, int bufferSize, sockaddr_in &addr);
};


#endif //HOMEWORK_01_UTIL_H
