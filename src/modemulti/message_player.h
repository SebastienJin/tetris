#ifndef TETRIS_PLAYER_H
#define TETRIS_PLAYER_H


#include "message.h"


class GetPlayerData : public Message 
{
public:
    void codemessage(std::vector<char> &data) override;
    void decodemessage(std::vector<char> data) override;
    messageType getType() override;
};


#endif //TETRIS_PLAYER_H
