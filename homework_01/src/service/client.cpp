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

    int openFileSuccess = 0;

    read(server_fd, &openFileSuccess, sizeof(int));

    if (openFileSuccess == 0) {
        printf("Server could not open the file. Closing connection...\n");
        close(server_fd);
        return;
    }

    printf("Server successfully opening the file. The server will now attempt to send the file\n");

    int chunks;
    read(server_fd, &chunks, sizeof(int));
    printf("The server will send %d chunks of %db\n", chunks, BYTES);

    /* Creating the file */
    int fd = open("file.mkv", O_CREAT | O_WRONLY, 0777);

    for (int i = 0; i < chunks; i++) {
        int bytesRead;
        read(server_fd, &bytesRead, sizeof(int));

        read(server_fd, buffer, bytesRead);
        write(fd, buffer, bytesRead);
    }

    close(fd);

    printf("Done\n");

    close(server_fd);
}

void Client::startUDP(char *ip, int port) {

}