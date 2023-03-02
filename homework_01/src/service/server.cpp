#include "server.h"

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <thread>
#include <fcntl.h>
#include "../const/const.h"
#include "util.h"

using namespace std;

void Server::startTCP(int port) {
    int server_fd;

    struct sockaddr_in address;

    int opt = 1;
    int addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET,SO_REUSEADDR | SO_REUSEPORT, &opt,sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr*)&address,sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    int clientId = 1;

    printf("The server is running at 127.0.0.1:%d\n\n", port);

    while (true) {
        int client_fd;

        if ((client_fd = accept(server_fd, (struct sockaddr*) &address,(socklen_t*) &addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        printf("Starting thread\n");

        /* Start a new thread handling the client */
        new thread(serverJob, client_fd, clientId);

        clientId++;
    }

    // closing the listening socket
    shutdown(server_fd, SHUT_RDWR);
}

void Server::serverJob(int client_fd, int client_id) {
    printf("[Client %d] Running job for client\n", client_id);

    char folderPath[1024] = "server_files";

    int fileCount = Util::getFileCount(folderPath);

    printf("[Client %d] Files to be sent %d\n", client_fd, fileCount);
    write(client_fd, &fileCount, sizeof(int));

    char buffer[BYTES];

    char okMessage[8] = "OK";
    char failMessage[8] = "FAIL";

    char** files = Util::getFilesFromDirectory(folderPath);
    for (int i = 0; i < fileCount; i++) {
        char* filePath = files[i];
        char* fileName = Util::getFileNameFromPath(filePath);

        write(client_fd, fileName, 512);

        int fd = open(filePath, O_RDONLY);

        int openSuccess = 1;
        if (fd < 0) {
            printf("Error opening file %s\n", fileName);
            openSuccess = 0;
        }

        printf("[Client %d] Sending confirmation regarding the file opening\n", client_id);
        write(client_fd, &openSuccess, sizeof(int));

        if (openSuccess == 0) {
            continue;
        }

        long int fileSize = Util::getFileSize(filePath);
        int chunks = (fileSize / BYTES) + (fileSize % BYTES != 0);

        printf("[Client %d] File size %ld. Sending the number of chunks: %d\n", client_id, fileSize, chunks);
        write(client_fd, &chunks, sizeof(int));

        /* Sending the file */
        int packageCountConfirm;
        for (int i = 1; i <= chunks; i++) {
            int bytesRead = read(fd, buffer, BYTES);
            write(client_fd, &bytesRead, sizeof(bytesRead));

            /* Read the confirmation of receiving the package */
            read(client_fd, &packageCountConfirm, sizeof(int));

            printf("[Client %d][%s][%.2f%%] Sending package %d of %dB with confirmation: %s\n", client_id,
                   fileName, 1.0f * i / chunks * 100, i, BYTES, packageCountConfirm == i ? okMessage : failMessage);
            write(client_fd, buffer, bytesRead);
        }

        close(fd);
    }

//    free(files);
    printf("Done. Closing connection with client %d...\n", client_id);
    close(client_fd);
}

void Server::startUDP(int port) {

}