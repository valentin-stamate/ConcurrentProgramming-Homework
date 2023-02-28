#include<iostream>
#include "service/server.h"

int main() {
    printf("Server\n");

    int port = 8080;

    Server server;
    server.startTCP(port);

    return 0;
}