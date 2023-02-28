#ifndef HOMEWORK_01_CLIENT_H
#define HOMEWORK_01_CLIENT_H

#include <string>
using namespace std;

class Client {
private:

public:
    void startTCP(char* ip, int port);
    void startUDP(char* ip, int port);
};

#endif //HOMEWORK_01_CLIENT_H
