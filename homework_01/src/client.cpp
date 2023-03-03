#include "service/client.h"
#include "const/const.h"

int main() {
    const char ip[64] = "127.0.0.1";
    int port = 10000;
    int packageSize = 16;
    int datasetType = DATASET_01;

    Client client(ip, port, packageSize, datasetType);
    client.startTCP();

    return 0;
}