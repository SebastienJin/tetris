#ifndef PLAYER_H
#define PLAYER_H


#include <string>

#include "message.h"
#include "../game_logic.h"


class Player : public Message 
{
public:

    int client_socket;
    std::string name = "unknown";
    unsigned level;
    unsigned score;
    unsigned completed_lines;
    bool alive;
    bool connected;
    
    explicit Player(int socket);

    void codemessage(std::vector<char> &data) override;
    void decodemessage(std::vector<char> data) override;
    messageType getType() override;
    void setName(std::string n);
    void update(Game& g);
    void update(Player* p);

};

struct player {
    unsigned level;
    unsigned score;
    unsigned completed_lines;
    bool alive;
    char name[];
};


#endif //PLAYER_H
