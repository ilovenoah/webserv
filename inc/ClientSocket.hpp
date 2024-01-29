#ifndef CLIENTSOCKET_HPP
#define CLIENTSOCKET_HPP

#include <poll.h>
#include <sys/socket.h>
#include "utils.hpp"

#define BUFFERSIZE 4096

class ClientSocket {
    private:
        int _fd;
        short _revents;
    public:
        ClientSocket();
        ClientSocket(int const fd);
        int getFd() const;
        void setRevents(short revents);
        bool tryRecv();
        bool trySend();
};

#endif