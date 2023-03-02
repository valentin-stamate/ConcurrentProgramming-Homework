#include<iostream>
#include "service/client.h"

int main() {
    char ip[] = "127.0.0.1";
    int port = 8080;

    Client client;
    client.startTCP(ip, port);

    return 0;
}