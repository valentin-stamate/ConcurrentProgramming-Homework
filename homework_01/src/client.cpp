#include "service/client.h"
#include "const/const.h"

int main(int argc, char** argv) {
    int protocol = atoi(argv[1]);
    const char* ip = argv[2];
    printf("%s\n", ip);
    int port = atoi(argv[3]);
    int packageSize = atoi(argv[4]);
    int datasetType = atoi(argv[5]);
    int acknowledge = atoi(argv[6]);

    Client client(ip, port, packageSize, datasetType, acknowledge);

    if (protocol == TCP) {
        client.startTCP();
    } else if (protocol == UDP) {
        client.startUDP();
    }

    return 0;
}