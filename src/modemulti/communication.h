#ifndef TETRIS_COMMUNICATOR_H
#define TETRIS_COMMUNICATOR_H


#include <sys/socket.h>
#include <iostream>

#include "message.h"


namespace com {

    void sendMsg(int socket, Message& msg) noexcept(false);
    bool data_nonempty(int socket);
    void receiveData(int socket, std::vector<char>& buffer) noexcept(false);
    
}


#endif //TETRIS_COMMUNICATOR_H
