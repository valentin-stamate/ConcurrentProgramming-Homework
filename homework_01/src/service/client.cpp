#include "client.h"

#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include "../const/const.h"

void Client::startTCP(char* ip, int port) {
    int server_fd;

    struct sockaddr_in serv_addr;

    char buffer[BYTES] = {0};

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

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

        printf("File %s opened successfully\n", fileName);

        int chunks;
        read(server_fd, &chunks, sizeof(int));
        printf("The server will send %d chunks of %db\n", chunks, BYTES);

        /* Creating the file */
        char filePath[1024];
        sprintf(filePath, "%s/%s", this->filesPath, fileName);
        int fd = open(filePath, O_CREAT | O_WRONLY, 0777);

        for (int j = 1; j <= chunks; j++) {
            int bytesRead;
            read(server_fd, &bytesRead, sizeof(int));

            /* Sending the confirmation of receiving the package */
            write(server_fd, &j, sizeof(int));

            printf("[%s][%.2f%%] Read package %d of %dB\n", fileName, 1.0f * j / chunks * 100, j, BYTES);
            read(server_fd, buffer, bytesRead);
//            printf("Write to local package %d to file %s\n", j, fileName);
            write(fd, buffer, bytesRead);
        }

        close(fd);
    }

    printf("Done. Closing connection...\n");
    close(server_fd);
}

void Client::startUDP(char *ip, int port) {

}