#include "server.h"

#include <netinet/in.h>
#include <cstdio>
#include <cstdlib>
#include <sys/socket.h>
#include <unistd.h>
#include <thread>
#include <fcntl.h>
#include <cstring>
#include "util.h"
#include "../const/const.h"
#include <sys/time.h>

using namespace std;

char Server::filesPath[64] = "/home/app/server_files";
char Server::dataset_0[64] = "/home/app/server_files/dataset_0";

Server::Server(int port, int protocol): port(port), protocol(protocol) { }

void Server::startTCP() {
    int server_fd;

    struct sockaddr_in address;

    int opt = 1;
    int addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port
    if (setsockopt(server_fd, SOL_SOCKET,SO_REUSEADDR | SO_REUSEPORT, &opt,sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    // Forcefully attaching socket to the port
    if (bind(server_fd, (struct sockaddr*)&address,sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    int clientId = 1;

    printf("TCP server is running at 127.0.0.1:%d\n\n", port);

    while (true) {
        int client_fd;

        if ((client_fd = accept(server_fd, (struct sockaddr*) &address,(socklen_t*) &addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        printf("Starting thread\n");

        /* Start a new thread handling the client */
        new thread(startJob, protocol, clientId, client_fd, sockaddr_in());

        clientId++;
    }

    // closing the listening socket
    shutdown(server_fd, SHUT_RDWR);
}

void Server::startUDP() {
    printf("UDP server is running at 127.0.0.1:%d\n\n", port);

    int client_fd;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;

    // Creating socket file descriptor
    if ((client_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));

    // Filling server information
    server_addr.sin_family = AF_INET; // IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(this->port);

    // Bind the socket with the server address
    if (bind(client_fd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    while (true) {
        Server::startJob(protocol, 1, client_fd, client_addr);
    }

    close(client_fd);
}

void Server::startJob(int protocol, int client_id, int client_fd, sockaddr_in client_addr) {
    printf("[Client %d] Running job for client\n", client_id);

    int packagesSend = 0;
    int packagesReceived = 0;
    int bytesSend = 0;
    int bytesReceived = 0;

    int PACKAGE_SIZE;
    Util::readFrom(protocol, client_fd, &PACKAGE_SIZE, sizeof(int), client_addr, &packagesReceived, &bytesReceived);
    char buffer[PACKAGE_SIZE];
    printf("[Client %d] Client requests packages of %dB\n", client_id, PACKAGE_SIZE);

    int DATASET_TYPE;
    Util::readFrom(protocol, client_fd, &DATASET_TYPE, sizeof(int), client_addr, &packagesReceived, &bytesReceived);
    printf("[Client %d] Requested dataset: %d\n", client_id, DATASET_TYPE);

    int acknowledge;
    Util::readFrom(protocol, client_fd, &acknowledge, sizeof(int), client_addr, &packagesReceived, &bytesReceived);
    printf("[Client %d] Acknowledgement: %d\n", client_id, acknowledge);

    char folderPath[1024];
    sprintf(folderPath, "/home/app/server_files/dataset_0%d", DATASET_TYPE);

    int fileCount = Util::getFileCount(folderPath);

    printf("[Client %d] Files to be sent %d\n", client_fd, fileCount);
    Util::writeTo(protocol, client_fd, &fileCount, sizeof(int), client_addr, &packagesSend, &bytesSend);

    char okMessage[8] = "OK";
    char failMessage[8] = "FAIL";

    char** files = Util::getFilesFromDirectory(folderPath);

    for (int i = 0; i < fileCount; i++) {
        char* filePath = files[i];
        char* fileName = Util::getFileNameFromPath(filePath);

        Util::writeTo(protocol, client_fd, fileName, 512, client_addr, &packagesSend, &bytesSend);

        int fd = open(filePath, O_RDONLY);

        int openSuccess = 1;
        if (fd < 0) {
            printf("Error opening file %s\n", fileName);
            openSuccess = 0;
        }

//        printf("[Client %d] Sending confirmation regarding the file opening\n", client_id);
        Util::writeTo(protocol, client_fd, &openSuccess, sizeof(int), client_addr, &packagesSend, &bytesSend);

        if (openSuccess == 0) {
            continue;
        }

        long int fileSize = (int) Util::getFileSize(filePath);
        int packages = (int) (fileSize / PACKAGE_SIZE) + (fileSize % PACKAGE_SIZE != 0);

        Util::writeTo(protocol, client_fd, &fileSize, sizeof(int), client_addr, &packagesSend, &bytesSend);

        printf("[Client %d] File size %ld. Sending the number of packages: %d\n", client_id, fileSize, packages);
        Util::writeTo(protocol, client_fd, &packages, sizeof(int), client_addr, &packagesSend, &bytesSend);

        /* Sending the file */
        int packageCountConfirm;
        for (int j = 1; j <= packages; j++) {
            int bytesRead = read(fd, buffer, PACKAGE_SIZE);
            Util::writeTo(protocol, client_fd, &bytesRead, sizeof(bytesRead), client_addr, &packagesReceived, &bytesReceived);

            /* Read the confirmation of receiving the package */
            if (acknowledge == 1) {
                Util::readFrom(protocol, client_fd, &packageCountConfirm, sizeof(int), client_addr, &packagesReceived, &bytesReceived);
            }

//            printf("[Client %d][%s][%.2f%%] Sending package %d of %dB with confirmation: %s\n", client_id,
//                   fileName, 1.0f * j / packages * 100, j, PACKAGE_SIZE, packageCountConfirm == j ? okMessage : failMessage);
            Util::writeTo(protocol, client_fd, buffer, bytesRead, client_addr, &packagesSend, &bytesSend);
//            usleep(20);
        }

        close(fd);
    }

//    free(files);
//    free(buffer);
    printf("[Client %d] Done. Protocol: %s. Dataset %d. Packages sent %d. Packages received %d. Bytes send %d. Bytes received %d",
           client_id, protocol == TCP ? "TCP" : "UDP", DATASET_TYPE, packagesSend, packagesReceived, bytesSend, bytesReceived);
    close(client_fd);
}