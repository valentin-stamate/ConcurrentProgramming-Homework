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

    printf("Protocol: %d. Port: %d. Package size: %d. Dataset: %d. Acknowledge: %d\n",
           protocol, port, packageSize, datasetType, acknowledge);

    Client client(ip, port, packageSize, datasetType, acknowledge);

    if (protocol == TCP) {
        client.startTCP();
    } else if (protocol == UDP) {
        client.startUDP();
    } else {
        printf("Something went wrong\n");
    }

    return 0;
}