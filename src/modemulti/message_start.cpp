#include "message_start.h"

GameStart::GameStart(long seed) : seed(seed) {}

void GameStart::codemessage(std::vector<char> &data) 
{
    data.resize(SIZE_OF_MESSAGE_SIZE + 1 + sizeof(seed));
    auto* ptr = reinterpret_cast<gamestart *>(data.data() + SIZE_OF_MESSAGE_SIZE + 1);
    ptr->seed = seed;
}

void GameStart::decodemessage(std::vector<char> data) 
{
    auto* ptr = reinterpret_cast<gamestart *>(data.data());
    seed = ptr->seed;
}

messageType GameStart::getType() 
{
    return GAME_START;
}

