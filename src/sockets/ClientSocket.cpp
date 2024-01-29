#include "ClientSocket.hpp"

ClientSocket::ClientSocket() {}

ClientSocket::ClientSocket(int const fd) : _fd(fd), _phase(ClientSocket::RECV) {}

int ClientSocket::getFd() const {
    return this->_fd;
}

void ClientSocket::setRevents(short revents) {
	this->_revents = revents;
}

bool ClientSocket::tryRecv() {
    char buf[BUFFERSIZE];
    if ((this->_revents & POLLIN) != POLLIN) {
        return false;
    }
    std::memset(&buf, 0, sizeof(buf));
    if (recv(this->_fd, buf, BUFFERSIZE - 1, 0) == -1) {
        utils::putSysError("recv");
        this->_revents = 0;
        return false;
    } 
    this->_revents = 0;
    return true;
}

bool ClientSocket::trySend() {
    const char *msg = "HTTP/1.1 200 Ok\nContent-Length: 12\n\nHelloworld!";
    if ((this->_revents & POLLOUT) != POLLOUT) {
        return false;
    }
    if (send(this->_fd, msg, std::strlen(msg), MSG_DONTWAIT | MSG_NOSIGNAL) == -1) {
        utils::putSysError("send");
        this->_revents = 0;
        return false;
    }
    this->_revents = 0;
    return true;
}

void ClientSocket::setPhase(ClientSocket::csphase const phase) {
    this->_phase = phase;
}

ClientSocket::csphase ClientSocket::getPhase() const {
    return this->_phase;
}