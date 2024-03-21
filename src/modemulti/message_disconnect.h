#ifndef TETRIS_DISCONNECT_H
#define TETRIS_DISCONNECT_H


#include <string>

#include "message.h"


class Disconnect : public Message 
{
public:

    std::string info;

    Disconnect() = default;
    Disconnect(std::string info);

    void codemessage(std::vector<char> &data) override;
    void decodemessage(std::vector<char> data) override;
    messageType getType() override;

};


#endif //TETRIS_DISCONNECT_H
