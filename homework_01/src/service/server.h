#ifndef HOMEWORK_01_SERVER_H
#define HOMEWORK_01_SERVER_H

#include <netinet/in.h>

class Server {
private:
    int port;
public:
    Server(int port);

    void startTCP();
    void startUDP();

    static void jobTCP(int client_fd, int client_id);
    static void jobUDP(int client_id, int client_fd, sockaddr_in client_addr);
};


#endif //HOMEWORK_01_SERVER_H
