#include "message_player.h"


void GetPlayerData::codemessage(std::vector<char> &data)
{
    data.resize(SIZE_OF_MESSAGE_SIZE + 1);
}

void GetPlayerData::decodemessage(std::vector<char> data) {}

messageType GetPlayerData::getType()
{
    return GET_PLAYER_DATA;
}
