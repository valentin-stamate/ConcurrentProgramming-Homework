#include "service/server.h"

int main() {
    int port = 10000;

    Server server(port);
    server.startTCP();

    return 0;
}