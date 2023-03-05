#include <cstdlib>
#include "service/server.h"
#include "const/const.h"

int main(int argc, char **argv) {
    int protocol;

    protocol = atoi(argv[1]);

    if (protocol == TCP) {
        Server server(10000, protocol);
        server.startTCP();
    } else if (protocol == UDP) {
        Server server(10001, protocol);
        server.startUDP();
    }

    return 0;
}