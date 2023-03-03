#include "util.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <a.out.h>
#include <csignal>

char* Util::getIp() {
    char cmd[100];
    sprintf(cmd, "%s", "ifconfig | grep inet | grep 192 | xargs | cut -d' ' -f2");

    FILE *fdRet = popen(cmd, "r");

    char line[4096];

    char *text = (char*) malloc(100);
    bzero(text, 100);

    while (fgets(line, 4096, fdRet) != NULL) {
        strcat(text, line);
    }

    text[strlen(text) - 1] = '\0';

    return text;
}

long int Util::getFileSize(char *fileName) {
    FILE *fp = fopen(fileName, "r");

    if (fp==NULL)
        return -1;

    if (fseek(fp, 0, SEEK_END) < 0) {
        fclose(fp);
        return -1;
    }

    long size = ftell(fp);
    // release the resources when not required
    fclose(fp);
    return size;
}

char** Util::getFilesFromDirectory(char *path) {
    char command[1024];
    int fileCount = Util::getFileCount(path);

    sprintf(command, "find %s -type f", path);

    FILE *fp;

    fp = popen(command, "r");
    if (fp == NULL) {
        printf("Failed to run command\n" );
        return NULL;
    }

    char** fileList = (char**) malloc(fileCount * sizeof (char*));

    for (int i = 0; i < fileCount; i++) {
        fileList[i] = (char*) malloc(1024);
        fgets(fileList[i], 1024, fp);
        fileList[i][strlen(fileList[i]) - 1] = '\0';
    }

    pclose(fp);

    return fileList;
}

int Util::getFileCount(char *path) {
    char command[1024];

    sprintf(command, "find %s -type f | wc -l", path);

    FILE *fp;
    char buffer[1024];

    /* Open the command for reading. */
    fp = popen(command, "r");
    if (fp == NULL) {
        printf("Failed to run command\n" );
        exit(1);
    }

    fgets(buffer, sizeof(buffer), fp);
    pclose(fp);

    return atoi(buffer);
}

char* Util::getFileNameFromPath(char *path) {
    char* copy = strdup(path);
    char* token = strtok(copy, "/");
    char* lastPath;

    while (token != NULL) {
        lastPath = token;
        token = strtok(NULL, "/");
    }

    return lastPath;
}

void Util::removeFiles(char *path) {
    char command[512];
    sprintf(command, "rm -rf %s/*", path);
    execl(command, NULL);
}
