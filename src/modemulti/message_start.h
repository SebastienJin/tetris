#ifndef TETRIS_START_H
#define TETRIS_START_H


#include <vector>

#include "message.h"


struct gamestart {
    long seed;
};

class GameStart : public Message
{
public:

    long seed;
        
    GameStart() = default;
    explicit GameStart(long seed);

    void codemessage(std::vector<char> &data) override;
    void decodemessage(std::vector<char> data) override;
    messageType getType() override;

};


#endif //TETRIS_START_H
