#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#include "server.h"
#include "message.h"
#include "message_disconnect.h"
#include "message_start.h"
#include "message_player.h"
#include "../scene/GameWindow.h"


Server::Server(Game& g): game(g), self(tetro_socket) {}

void Server::connectToServer(std::string ip, std::string name)
{
    isServer = false;
    running = true;

    self.update(game);

    tetro_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (tetro_socket == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        throw std::system_error(errno, std::system_category(), "Failed to create socket.");
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_addr.s_addr = inet_addr(ip.c_str());  // loopback address for testing on same machine
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(2001);
    if (connect(tetro_socket, (sockaddr *) &serverAddr, sizeof(serverAddr)) < 0) {
        throw std::system_error(errno, std::system_category(), "Failed to connect to server.");
    }

    std::cout << "Connected to server" << std::endl;

    self.setName(std::move(name));

    try {
        com::sendMsg(tetro_socket, self);
    } catch (const std::system_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    serverThread = std::thread(&Server::receiveMsg, this);
    serverThread.detach();
}

void Server::startServer()
{
    isServer = true;
    running = true;

    self.update(game);

    sockaddr_in server_address{};

    if ((tetro_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cerr << "Failed to create socket." << std::endl;
        throw std::system_error(errno, std::system_category(), "Failed to create socket.");
    }

    int status = fcntl(tetro_socket, F_SETFL, fcntl(tetro_socket, F_GETFL, 0) | O_NONBLOCK);

    if (status == -1){
        std::cerr << "Failed to make socket non blocking." << std::endl;
        throw std::system_error(errno, std::system_category(), "Failed to make socket non blocking.");
    }

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(2001);
    if (bind(tetro_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        std::cerr << "Failed to bind socket." << std::endl;
        throw std::system_error(errno, std::system_category(), "Failed to bind socket.");
    }

    if (listen(tetro_socket, 3) < 0) {
        std::cerr << "Listening for connections failed." << std::endl;
        throw std::system_error(errno, std::system_category(), "Listening for connections failed.");
    }

    serverThread = std::thread(&Server::clientupdate, this);
    serverThread.detach();
}

void Server::disconnectFromServer()
{
    running = false;
    close(tetro_socket);
    clients.clear();
}

void Server::stopServer()
{
    running = false;

    Disconnect d("The server of the game has been gone...\n End of game!");
    broadcastData(d);

    for (auto client : clients)
        close(client->client_socket);

    close(tetro_socket);
    clients.clear();
}

Server::~Server() {
    if (isServer)
    {
        stopServer();
    }
    else
    {
        disconnectFromServer();
    }
}

void Server::clientupdate()
{
    long date = std::time(nullptr);

    while (running)
    {
        int client_socket;
        sockaddr_in client_address{};

        socklen_t client_address_size = sizeof(client_address);
        if ((client_socket = accept(tetro_socket, (struct sockaddr *) &client_address, &client_address_size)) >= 0) {
            if (game.game_state == BEFORE) {

                auto p = new Player(client_socket);

                clients.push_back(p);
                try {
                    com::sendMsg(p->client_socket, self);
                } catch (const std::system_error& e) {
                    std::cerr << "Error: " << e.what() << std::endl;
                }
            } else {

                Disconnect packet("The game has already started");
                try {
                    com::sendMsg(client_socket, packet);
                } catch (const std::system_error& e) {
                    std::cerr << "Error: " << e.what() << std::endl;
					throw_error(e.what(), true);
                }
            }
        }

        for (auto client : clients)
        {
            if (client->connected && com::data_nonempty(client->client_socket) && running)
            {
                try {
                    std::vector<char> msg_size_as_char(SIZE_OF_MESSAGE_SIZE);
                    com::receiveData(client->client_socket, msg_size_as_char);

                    std::vector<char> msg_type_as_char(1);
                    com::receiveData(client->client_socket, msg_type_as_char);

                    int msg_size = atoi(msg_size_as_char.data());
                    auto msg_type = static_cast<messageType>((int) msg_type_as_char.at(0));

                    std::vector<char> buffer(msg_size - SIZE_OF_MESSAGE_SIZE - 1);
                    com::receiveData(client->client_socket, buffer);

                    if (msg_type == PLAYER_DATA)
                    {
                        client->decodemessage(buffer);

                        broadcastData(*client, client->client_socket);
                    }

                } catch (const std::exception& e) {
                    std::cerr << "Error: " << e.what() << std::endl;
                    client->connected = false;

                    close(client->client_socket);
                }
            }
        }

        long now = std::time(nullptr);
        if (now - date > 4 && running)
        {
            actualizePlayers();
            date = std::time(nullptr);
        }

    }
}

void Server::receiveMsg()
{
    while (running)
    {
        while (!com::data_nonempty(tetro_socket)) {}

        try {
            std::vector<char> msg_size_as_char(SIZE_OF_MESSAGE_SIZE);
            com::receiveData(tetro_socket, msg_size_as_char);

            std::vector<char> msg_type_as_char(1);
            com::receiveData(tetro_socket, msg_type_as_char);

            int msg_size = atoi(msg_size_as_char.data());
            auto msg_type = static_cast<messageType>((int) msg_type_as_char.at(0));

            std::vector<char> buffer(msg_size - SIZE_OF_MESSAGE_SIZE - 1);
            com::receiveData(tetro_socket, buffer);

            switch (msg_type) {
                case GAME_START: {
                    auto gs = GameStart();
                    gs.decodemessage(buffer);

                    game.Game_Start(gs.seed);
                    break;
                }
                case PLAYER_DATA: {
                    auto new_player = new Player(tetro_socket);
                    new_player->decodemessage(buffer);

                    bool found = false;

                    for (auto client: clients) {
                        if (client->name == new_player->name) {

                            client->update(new_player);

                            found = true;
                            break;
                        }
                    }

                    if (!found && self.name != new_player->name) {
                        clients.push_back(new_player);
                    }

                    break;
                }
                case DISCONNECT: {
                    Disconnect packet{};
                    packet.decodemessage(buffer);
                    disconnectFromServer();
                    break;
                }
                case GET_PLAYER_DATA:

                    if (self.alive)
                    {
                        self.update(game);
                        try {
                            com::sendMsg(tetro_socket, self);
                        } catch (const std::system_error& e) {
                            std::cerr << "Error can't send player data" << std::endl;
                        }
                    }
                    break;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            std::cout << "Can't communicate with server" << std::endl;
            disconnectFromServer();
        }
    }
}

bool Server::broadcastData(Message& msg, int client_socket)
{
    for (auto client : clients) {
        try {
            if (client->connected && client_socket != client->client_socket) {
                com::sendMsg(client->client_socket, msg);
            }
        } catch (const std::system_error &e) {
            std::cerr << "Error: " << e.what() << std::endl;
			throw_error(e.what(), true);
            return false;
        }
    }
    return true;
}

void Server::startGame(long seed)
{
    auto msg = GameStart(seed);

    broadcastData(msg);

    actualizePlayers();
}

void Server::actualizePlayers()
{
    self.update(game);
    broadcastData(self);

    GetPlayerData a;
    broadcastData(reinterpret_cast<Message &>(a));
}
