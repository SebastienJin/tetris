#include "message_disconnect.h"


Disconnect::Disconnect(std::string Info): info(Info) {}

messageType Disconnect::getType() 
{
    return DISCONNECT;
}

void Disconnect::codemessage(std::vector<char> &data) 
{
    data.resize(SIZE_OF_MESSAGE_SIZE + 1 + (info.size() + 1));
    for (int i = 0; i < info.size(); i++)
    {
        data[i + SIZE_OF_MESSAGE_SIZE + 1] = info[i];
    }
    data[info.size() + SIZE_OF_MESSAGE_SIZE + 1] = '\0';
}

void Disconnect::decodemessage(std::vector<char> data) 
{
    char* name = data.data();
    info = name;
}
