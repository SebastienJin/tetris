#include <cmath>

#include "message.h"


void Message::addSizeType(std::vector<char> &data)
{
    codemessage(data);

    int size = data.size();
    for (int i = 0; i < SIZE_OF_MESSAGE_SIZE; i++)
    {
        int c = size/pow(10, SIZE_OF_MESSAGE_SIZE - i - 1);
        data[i] = c + '0';

        size -= c*pow(10, SIZE_OF_MESSAGE_SIZE - i - 1);
    }

    data[4] = getType();
}

messageType Message::getType() 
{
    return UNKNOWN;
}
