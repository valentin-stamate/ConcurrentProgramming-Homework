#include<iostream>
#include "service/client.h"
#include <cstring>

int main() {
    printf("Client\n");

    char ip[] = "127.0.0.1";
    int port = 8080;

    Client client;
    client.startTCP(ip, port);

    return 0;
}