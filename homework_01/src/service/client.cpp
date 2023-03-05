#include "client.h"

#include <netinet/in.h>
#include <cstdio>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include "../const/const.h"
#include "util.h"

Client::Client(const char *ip, int port, int packageSize, int datasetType, int acknowledge):
        ip(ip), port(port), PACKAGE_SIZE(packageSize), DATASET_TYPE(datasetType), acknowledge(acknowledge) { }

void Client::startTCP() {
    int server_fd;

    struct sockaddr_in serv_addr;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(this->port);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, strdup(ip), &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return;
    }

    if (connect(server_fd, (struct sockaddr*)&serv_addr,sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return;
    }

    printf("Successfully connected with the server\n\n");

    startJob(TCP, server_fd, {});

    printf("Done. Closing connection...\n");
    close(server_fd);
}

void Client::startUDP() {
    int server_fd;

    struct sockaddr_in server_addr;

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));

    // Filling server information
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(this->port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    startJob(UDP, server_fd, server_addr);

    printf("Done\n");
    close(server_fd);
}

void Client::startJob(int protocol, int server_fd, sockaddr_in server_addr) {
    Util::removeFiles(filesPath);

    char buffer[PACKAGE_SIZE];
    socklen_t len = sizeof(server_addr);

    sendto(server_fd, &this->PACKAGE_SIZE, sizeof(int), MSG_CONFIRM, (const struct sockaddr *) &server_addr, len);
    printf("The desired size of future packages was send %dB\n", PACKAGE_SIZE);

    sendto(server_fd, &this->DATASET_TYPE, sizeof(int), MSG_CONFIRM, (const struct sockaddr *) &server_addr, len);
    printf("Sending the requested dataset: %d\n", this->DATASET_TYPE);

    int filesCount;
    recvfrom(server_fd, &filesCount, sizeof(int), MSG_WAITALL, (struct sockaddr *) &server_addr,&len);
    printf("Server files to be received %d\n", filesCount);

    for (int i = 0; i < filesCount; i++) {
        char fileName[512];
        Util::readFrom(protocol, server_fd, &fileName, 512, server_addr);

        int openSuccess;
        Util::readFrom(protocol, server_fd, &openSuccess, sizeof(int), server_addr);

        if (openSuccess == 0) {
            printf("Error opening %s\n", fileName);
            continue;
        }

        int fileSize;
        Util::readFrom(protocol, server_fd, &fileSize, sizeof(int), server_addr);

//        printf("File %s opened successfully\n", fileName);

        int chunks;
        Util::readFrom(protocol, server_fd, &chunks, sizeof(int), server_addr);
        printf("Receiving %s of %d. %d packages of %dB to be received....\n", fileName, fileSize, chunks, PACKAGE_SIZE);

        /* Creating the file */
        char filePath[1024];
        sprintf(filePath, "%s/%s", this->filesPath, fileName);
        int fd = open(filePath, O_CREAT | O_WRONLY, 0777);

        for (int j = 1; j <= chunks; j++) {
            int bytesRead;
            Util::readFrom(protocol, server_fd, &bytesRead, sizeof(int), server_addr);

            /* Sending the confirmation of receiving the package */
            Util::writeTo(protocol, server_fd, &j, sizeof(int), server_addr);

//            printf("[%s][%.2f%%] Read package %d of %dB\n", fileName, 1.0f * j / chunks * 100, j, bytesRead);

            Util::readFrom(protocol, server_fd, buffer, bytesRead, server_addr);
            write(fd, buffer, bytesRead);
        }

        close(fd);
    }
}
