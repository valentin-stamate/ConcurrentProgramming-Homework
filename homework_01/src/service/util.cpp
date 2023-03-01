#include "util.h"
#include <cstring>
#include <stdio.h>
#include <stdlib.h>

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
