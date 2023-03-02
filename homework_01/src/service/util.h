#ifndef HOMEWORK_01_UTIL_H
#define HOMEWORK_01_UTIL_H


class Util {
public:
    static char* getIp();
    static long int getFileSize(char* fileName);
    static char** getFilesFromDirectory(char* path);
    static int getFileCount(char* path);
    static char* getFileNameFromPath(char* path);
};


#endif //HOMEWORK_01_UTIL_H
