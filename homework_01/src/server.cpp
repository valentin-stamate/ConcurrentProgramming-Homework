#include "service/server.h"

int main() {
    int port = 8080;

    Server server;
    server.startTCP(port);

    return 0;
}