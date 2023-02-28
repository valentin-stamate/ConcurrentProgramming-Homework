#include "client.h"

#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "../const/const.h"

void Client::startTCP(char* ip, int port) {
    int status;
    int valread;
    int client_fd;

    struct sockaddr_in serv_addr;

    char* hello = strdup("Hello from client");
    char buffer[BYTES] = {0};

    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
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

    if ((status = connect(client_fd, (struct sockaddr*)&serv_addr,sizeof(serv_addr))) < 0) {
        printf("\nConnection Failed \n");
        return;
    }

    send(client_fd, hello, strlen(hello), 0);
    printf("Hello message sent\n");

    read(client_fd, buffer, BYTES);
    printf("%s\n", buffer);

    // closing the connected socket
    close(client_fd);
}

void Client::startUDP(char *ip, int port) {

}