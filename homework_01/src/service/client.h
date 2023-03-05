#ifndef HOMEWORK_01_CLIENT_H
#define HOMEWORK_01_CLIENT_H

#include <string>
#include <netinet/in.h>

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

    void jobTCP(int server_fd);
    void jobUDP(int server_fd, sockaddr_in server_addr);
};

#endif //HOMEWORK_01_CLIENT_H
