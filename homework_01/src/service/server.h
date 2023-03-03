#ifndef HOMEWORK_01_SERVER_H
#define HOMEWORK_01_SERVER_H


class Server {
private:
    int port;
public:
    Server(int port);

    void startTCP();
    void startUDP();

    static void serverJob(int client_fd, int client_id);
};


#endif //HOMEWORK_01_SERVER_H
