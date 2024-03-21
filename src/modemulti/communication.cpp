#include <vector>
#include <poll.h>

#include "communication.h"


namespace com {

    void sendMsg(int socket, Message& msg)
    {
        std::vector<char> data;
        msg.addSizeType(data);

        if (send(socket, data.data(), data.size(), 0) < 0)
        {
            throw std::system_error(errno, std::system_category(), "Can't send data");
        }
    }

    bool data_nonempty(int socket)
    {
        auto ufds = new pollfd;
        ufds->fd = socket;
        ufds->events = POLLIN;

        return poll(ufds, 1, 1000) > 0;
    }

    void receiveData(int socket, std::vector<char>& buffer)
    {
        if (recv(socket, buffer.data(), buffer.size(), 0) < 0)
        {
            throw std::system_error(errno, std::system_category(), "Can't receive data");
        }
    }

}