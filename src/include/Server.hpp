#include "KVStore.hpp"
#include <string>
#include <netinet/in.h>

class Server {
public:
    Server(int port);
    void start();
    
private:
    void handleClient(int clientSocket);
    void processCommand(const std::string& command, int clientSocket);
    
    KVStore store;
    int serverSocket;
    int port;
    struct sockaddr_in serverAddr;
};