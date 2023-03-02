#ifndef HOMEWORK_01_SERVER_H
#define HOMEWORK_01_SERVER_H


class Server {
private:
public:
    void startTCP(int port);
    void startUDP(int port);

    static void serverJob(int client_fd, int client_id);
};


#endif //HOMEWORK_01_SERVER_H
