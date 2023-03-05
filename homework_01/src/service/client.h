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
    int PACKAGE_SIZE = 1024;
    int DATASET_TYPE;
    int acknowledge;
public:
    Client(const char *ip, int port, int packageSize, int datasetType, int acknowledge);

    void startTCP();
    void startUDP();

    void startJob(int protocol, int server_fd, sockaddr_in server_addr);
};

#endif //HOMEWORK_01_CLIENT_H
