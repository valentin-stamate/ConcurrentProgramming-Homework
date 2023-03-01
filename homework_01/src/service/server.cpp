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

    char fileName[] = "files/video.mkv";

    char buffer[BYTES] = "";
    int fd = open(fileName, O_RDONLY);

    int openSuccess = 0;
    if (fd < 0) {
        printf("Error opening file\n");
        close(client_fd);
        return;
    }

    openSuccess = 1;

    printf("[Client %d] Sending confirmation regarding the file opening\n", client_id);
    write(client_fd, &openSuccess, sizeof(int));

    long int fileSize = Util::getFileSize(fileName);
    int chunks = (fileSize / BYTES) + (fileSize % BYTES != 0);

    printf("[Client %d] File size %ld. Sending the number of chunks: %d\n", client_id, fileSize, chunks);
    write(client_fd, &chunks, sizeof(int));

    /* Sending the file */
    for (int i = 0; i < chunks; i++) {
        int bytesRead = read(fd, buffer, BYTES);
        write(client_fd, &bytesRead, sizeof(bytesRead));

        write(client_fd, buffer, bytesRead);
    }

    close(fd);

    printf("Done\n");

//    printf("Readdmi\n");
//    read(client_fd, buffer, BYTES);
//    printf("%s\n", buffer);
//
//    printf("Sending\n");
//    send(client_fd, message, BYTES, 0);
//    printf("Hello message sent\n");

    close(client_fd);
}

void Server::startUDP(int port) {

}