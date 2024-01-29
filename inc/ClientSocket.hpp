#ifndef CLIENTSOCKET_HPP
#define CLIENTSOCKET_HPP

#include <poll.h>
#include <sys/socket.h>
#include "utils.hpp"

#define BUFFERSIZE 4096

class ClientSocket {
    public:
        enum csphase {
            RECV,
            SEND,
            CLOSE
        };

    private:
        int _fd;
        short _revents;
        ClientSocket::csphase _phase;

    public:
        ClientSocket();
        ClientSocket(int const fd);
        int getFd() const;
        void setRevents(short revents);
        short getRevents() const;
        ClientSocket::csphase tryRecv();
        ClientSocket::csphase trySend();
        void setPhase(ClientSocket::csphase const phase);
        ClientSocket::csphase getPhase() const;
};

#endif