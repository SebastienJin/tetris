#ifndef TETRIS_MESSAGE_H
#define TETRIS_MESSAGE_H


#include <vector>

#define SIZE_OF_MESSAGE_SIZE 4


enum messageType {
    GAME_START, 
    PLAYER_DATA, 
    DISCONNECT, 
    GET_PLAYER_DATA, 
    UNKNOWN
};

class Message 
{
public:

    virtual messageType getType();
    void addSizeType(std::vector<char> &data);
    virtual void codemessage(std::vector<char> &data) = 0;
    virtual void decodemessage(std::vector<char> data) = 0;

};


#endif //TETRIS_MESSAGE_H
