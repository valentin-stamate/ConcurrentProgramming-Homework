#include "server.h"

#include <netinet/in.h>
#include <cstdio>
#include <cstdlib>
#include <sys/socket.h>
#include <unistd.h>
#include <thread>
#include <fcntl.h>
#include "../const/const.h"
#include "util.h"

using namespace std;

Server::Server(int port) {
    this->port = port;
}

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
    char filesPath[64] = "server_files";
    char dataset_01[64] = "dataset_01";
    char dataset_02[64] = "dataset_02";

    printf("[Client %d] Running job for client\n", client_id);

    int PACKAGE_SIZE;
    read(client_fd, &PACKAGE_SIZE, sizeof(int));
    printf("[Client %d] Client requests packages of %dB\n", client_id, PACKAGE_SIZE);

    int DATASET_TYPE;
    read(client_fd, &DATASET_TYPE, sizeof(int));
    printf("[Client %d] Requested dataset: %d\n", client_id, DATASET_TYPE);

    char folderPath[1024];
    sprintf(folderPath, "%s/%s", filesPath, DATASET_TYPE == 1 ? dataset_01 : dataset_02);

    int fileCount = Util::getFileCount(folderPath);

    printf("[Client %d] Files to be sent %d\n", client_fd, fileCount);
    write(client_fd, &fileCount, sizeof(int));

    char okMessage[8] = "OK";
    char failMessage[8] = "FAIL";

    char* buffer = (char*) malloc(PACKAGE_SIZE * sizeof(char));

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

        long int fileSize = (int) Util::getFileSize(filePath);
        int chunks = (int) (fileSize / PACKAGE_SIZE) + (fileSize % PACKAGE_SIZE != 0);

        write(client_fd, &fileSize, sizeof(int));

        printf("[Client %d] File size %ld. Sending the number of chunks: %d\n", client_id, fileSize, chunks);
        write(client_fd, &chunks, sizeof(int));

        /* Sending the file */
        int packageCountConfirm;
        for (int j = 1; j <= chunks; j++) {
            int bytesRead = read(fd, buffer, PACKAGE_SIZE);
            write(client_fd, &bytesRead, sizeof(bytesRead));

            /* Read the confirmation of receiving the package */
            read(client_fd, &packageCountConfirm, sizeof(int));

            printf("[Client %d][%s][%.2f%%] Sending package %d of %dB with confirmation: %s\n", client_id,
                   fileName, 1.0f * j / chunks * 100, j, PACKAGE_SIZE, packageCountConfirm == j ? okMessage : failMessage);
            write(client_fd, buffer, bytesRead);
        }

        close(fd);
    }

//    free(files);
    free(buffer);
    printf("Done. Closing connection with client %d...\n", client_id);
    close(client_fd);
}