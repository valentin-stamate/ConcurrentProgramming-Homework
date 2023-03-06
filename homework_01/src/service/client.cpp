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
#include <sys/time.h>

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

    printf("Inside protocol: %d\n", protocol);

    struct timeval stop, start;
    gettimeofday(&start, NULL);
    int packagesSend = 0;
    int packagesReceived = 0;
    int bytesSend = 0;
    int bytesReceived = 0;

    char buffer[PACKAGE_SIZE];

    Util::writeTo(protocol, server_fd, &this->PACKAGE_SIZE, sizeof(int), server_addr, &packagesSend, &bytesSend);
    printf("The desired size of future packages was send %dB\n", PACKAGE_SIZE);

    Util::writeTo(protocol, server_fd, &this->DATASET_TYPE, sizeof(int), server_addr, &packagesSend, &bytesSend);
    printf("Sending the requested dataset: %d\n", this->DATASET_TYPE);

    Util::writeTo(protocol, server_fd, &this->acknowledge, sizeof(int), server_addr, &packagesSend, &bytesSend);
    printf("Sending the acknowledgement: %d\n", this->acknowledge);

    int filesCount;
    Util::readFrom(protocol, server_fd, &filesCount, sizeof(int), server_addr, &packagesReceived, &bytesReceived);
    printf("Server files to be received %d\n", filesCount);

    for (int i = 0; i < filesCount; i++) {
        char fileName[512];
        Util::readFrom(protocol, server_fd, &fileName, 512, server_addr, &packagesReceived, &bytesReceived);

        int openSuccess;
        Util::readFrom(protocol, server_fd, &openSuccess, sizeof(int), server_addr, &packagesReceived, &bytesReceived);

        if (openSuccess == 0) {
            printf("Error opening %s\n", fileName);
            continue;
        }

        int fileSize;
        Util::readFrom(protocol, server_fd, &fileSize, sizeof(int), server_addr, &packagesReceived, &bytesReceived);

//        printf("File %s opened successfully\n", fileName);

        int packages;
        Util::readFrom(protocol, server_fd, &packages, sizeof(int), server_addr, &packagesReceived, &bytesReceived);
        printf("Receiving %s of %d. %d packages of %dB to be received....\n", fileName, fileSize, packages, PACKAGE_SIZE);

        /* Creating the file */
        char filePath[1024];
        sprintf(filePath, "%s/%s", this->filesPath, fileName);
        int fd = open(filePath, O_CREAT | O_WRONLY, 0777);

        for (int j = 1; j <= packages; j++) {
            int bytesRead;
            Util::readFrom(protocol, server_fd, &bytesRead, sizeof(int), server_addr, &packagesReceived, &bytesReceived);

            /* Sending the confirmation of receiving the package */
            if (acknowledge == 1) {
                Util::writeTo(protocol, server_fd, &j, sizeof(int), server_addr, &packagesSend, &bytesSend);
//                usleep(2000);
            } else {
//                usleep(250);
            }

//            printf("[%s][%.2f%%] Read package %d of %dB\n", fileName, 1.0f * j / packages * 100, j, bytesRead);

            Util::readFrom(protocol, server_fd, buffer, bytesRead, server_addr, &packagesReceived, &bytesReceived);
            write(fd, buffer, bytesRead);
        }

        close(fd);
    }

    gettimeofday(&stop, NULL);

    int dt = stop.tv_sec - start.tv_sec;

    printf("Done. Protocol: %s. Dataset %d. Transfer time %ds. Packages sent %d. Packages received %d. Bytes send %d. Bytes received %d\n",
           protocol == TCP ? "TCP" : "UDP", DATASET_TYPE, dt, packagesSend, packagesReceived, bytesSend, bytesReceived);
}
