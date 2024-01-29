#include "ClientSocket.hpp"

ClientSocket::ClientSocket() {}

ClientSocket::ClientSocket(int const fd) : _fd(fd) {}

int ClientSocket::getFd() const {
    return this->_fd;
}