#ifndef SERVER_H
#define SERVER_H


#include <vector>
#include <string>

#include "../game_logic.h"
#include "player.h"
#include "communication.h"


class Server
{
public:

    std::vector<Player *> clients;
    Player self;
    bool isServer = false;
    bool running = false;
    
    explicit Server(Game &g);
    ~Server();

    void connectToServer(std::string ip, std::string name);
    void disconnectFromServer();
    void startServer();
    void stopServer();
    void startGame(long seed);

private:

    std::thread serverThread;
    Game &game;    
    int tetro_socket;

    void clientupdate();
    void receiveMsg();
    bool broadcastData(Message &message, int client_socket = -1);
    void actualizePlayers();

};


#endif // SERVER_H
