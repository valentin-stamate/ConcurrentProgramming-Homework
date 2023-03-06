#ifndef HOMEWORK_01_SERVER_H
#define HOMEWORK_01_SERVER_H

#include <netinet/in.h>

class Server {
private:
    const int port;
    const int protocol;
public:
    static char filesPath[64];
    static char dataset_0[64];

    Server(int port, int protocol);

    void startTCP();
    void startUDP();

    static void startJob(int protocol, int client_id, int client_fd, sockaddr_in client_addr);
};


#endif //HOMEWORK_01_SERVER_H
