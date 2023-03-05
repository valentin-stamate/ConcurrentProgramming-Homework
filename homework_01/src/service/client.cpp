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

Client::Client(const char *ip, int port, int packageSize, int datasetType):
        ip(ip), port(port), PACKAGE_SIZE(packageSize), DATASET_TYPE(datasetType) { }

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

    jobTCP(server_fd);

    printf("Done. Closing connection...\n");
    close(server_fd);
}

void Client::jobTCP(int server_fd) {
    Util::removeFiles(filesPath);

    char buffer[PACKAGE_SIZE];

    write(server_fd, &this->PACKAGE_SIZE, sizeof(int));
    printf("The desired size of future packages was send %dB\n", PACKAGE_SIZE);

    write(server_fd, &this->DATASET_TYPE, sizeof(int));
    printf("Sending the requested dataset: %d\n", this->DATASET_TYPE);

    int filesCount;
    read(server_fd, &filesCount, sizeof(int));
    printf("Server files to be received %d\n", filesCount);

    for (int i = 0; i < filesCount; i++) {
        char fileName[512];
        read(server_fd, &fileName, 512);

        int openSuccess;
        read(server_fd, &openSuccess, sizeof(int));

        if (openSuccess == 0) {
            printf("Error opening %s\n", fileName);
            continue;
        }

        int fileSize;
        read(server_fd, &fileSize, sizeof(int));

//        printf("File %s opened successfully\n", fileName);

        int chunks;
        read(server_fd, &chunks, sizeof(int));
        printf("Receiving %s of %d. %d packages of %dB to be received....\n", fileName, fileSize, chunks, PACKAGE_SIZE);

        /* Creating the file */
        char filePath[1024];
        sprintf(filePath, "%s/%s", this->filesPath, fileName);
        int fd = open(filePath, O_CREAT | O_WRONLY, 0777);

        for (int j = 1; j <= chunks; j++) {
            int bytesRead;
            read(server_fd, &bytesRead, sizeof(int));

            /* Sending the confirmation of receiving the package */
            write(server_fd, &j, sizeof(int));

//            printf("[%s][%.2f%%] Read package %d of %dB\n", fileName, 1.0f * j / chunks * 100, j, bytesRead);

            read(server_fd, buffer, bytesRead);
            write(fd, buffer, bytesRead);
        }

        close(fd);
    }
}

void Client::startUDP() {
    int server_fd;

    const char *hello = "Hello from client";
    struct sockaddr_in     server_addr;

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

    jobUDP(server_fd, server_addr);

    printf("Done\n");
    close(server_fd);
}

void Client::jobUDP(int server_fd, sockaddr_in server_addr) {
    Util::removeFiles(filesPath);

    socklen_t len = sizeof(server_addr);

    char buffer[PACKAGE_SIZE];

    sendto(server_fd, &this->PACKAGE_SIZE, sizeof(int), MSG_CONFIRM, (const struct sockaddr *) &server_addr, len);
    printf("The desired size of future packages was send %dB\n", PACKAGE_SIZE);

    sendto(server_fd, &this->DATASET_TYPE, sizeof(int), MSG_CONFIRM, (const struct sockaddr *) &server_addr, len);
    printf("Sending the requested dataset: %d\n", this->DATASET_TYPE);

    int filesCount;
    recvfrom(server_fd, &filesCount, sizeof(int), MSG_WAITALL, (struct sockaddr *) &server_addr,&len);
    printf("Server files to be received %d\n", filesCount);

    for (int i = 0; i < filesCount; i++) {
        char fileName[512];
        recvfrom(server_fd, &fileName, 512,MSG_WAITALL, (struct sockaddr *) &server_addr,&len);

        int openSuccess;
        recvfrom(server_fd, &openSuccess, sizeof(int), MSG_WAITALL, (struct sockaddr *) &server_addr,&len);

        if (openSuccess == 0) {
            printf("Error opening %s\n", fileName);
            continue;
        }

        int fileSize;
        recvfrom(server_fd, &fileSize, sizeof(int), MSG_WAITALL, (struct sockaddr *) &server_addr,&len);

//        printf("File %s opened successfully\n", fileName);

        int chunks;
        recvfrom(server_fd, &chunks, sizeof(int), MSG_WAITALL, (struct sockaddr *) &server_addr,&len);
        printf("Receiving %s of %d. %d packages of %dB to be received....\n", fileName, fileSize, chunks, PACKAGE_SIZE);

        /* Creating the file */
        char filePath[1024];
        sprintf(filePath, "%s/%s", this->filesPath, fileName);
        int fd = open(filePath, O_CREAT | O_WRONLY, 0777);

        for (int j = 1; j <= chunks; j++) {
            int bytesRead;
            recvfrom(server_fd, &bytesRead, sizeof(int), MSG_WAITALL, (struct sockaddr *) &server_addr,&len);

            /* Sending the confirmation of receiving the package */
            sendto(server_fd, &j, sizeof(int), MSG_CONFIRM, (const struct sockaddr *) &server_addr, len);

//            printf("[%s][%.2f%%] Read package %d of %dB\n", fileName, 1.0f * j / chunks * 100, j, bytesRead);

            recvfrom(server_fd, buffer, bytesRead, MSG_WAITALL, (struct sockaddr *) &server_addr,&len);
            write(fd, buffer, bytesRead);
        }

        close(fd);
    }

}
