#ifndef HOMEWORK_01_CLIENT_H
#define HOMEWORK_01_CLIENT_H

#include <string>
using namespace std;

class Client {
private:
    char filesPath[64] = "client_files";
    const int port;
    const char *ip;
    const int PACKAGE_SIZE = 1024;
    const int DATASET_TYPE;
public:
    Client(const char *ip, int port, int packageSize, int datasetType);

    void startTCP();
    void startUDP();
};

#endif //HOMEWORK_01_CLIENT_H
