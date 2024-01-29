#include "ClientSocket.hpp"

ClientSocket::ClientSocket() {}

ClientSocket::ClientSocket(int const fd) : _fd(fd), _phase(ClientSocket::RECV) {}

int ClientSocket::getFd() const {
    return this->_fd;
}

void ClientSocket::setRevents(short revents) {
	this->_revents = revents;
}

short ClientSocket::getRevents() const {
    return this->_revents;
}

ClientSocket::csphase ClientSocket::tryRecv() {
    char buf[BUFFERSIZE];
    if ((this->_revents & POLLIN) != POLLIN) {
        return this->_phase;
    }
    std::memset(&buf, 0, sizeof(buf));
    if (recv(this->_fd, buf, BUFFERSIZE - 1, 0) == -1) {
        utils::putSysError("recv");
        return ClientSocket::CLOSE;
    } 
    return ClientSocket::SEND;
}

ClientSocket::csphase ClientSocket::trySend() {
    const char *msg = "HTTP/1.1 200 Ok\nContent-Length: 11\n\nHelloworld!";
    if ((this->_revents & POLLOUT) != POLLOUT) {
        return this->_phase;
    }
    if (send(this->_fd, msg, std::strlen(msg), MSG_DONTWAIT | MSG_NOSIGNAL) == -1) {
        utils::putSysError("send");
        return ClientSocket::CLOSE;
    }
    return ClientSocket::RECV;
}

void ClientSocket::setPhase(ClientSocket::csphase const phase) {
    this->_phase = phase;
}

ClientSocket::csphase ClientSocket::getPhase() const {
    return this->_phase;
}