#include "ClientSocket.hpp"

ClientSocket::ClientSocket() {}

ClientSocket::ClientSocket(int const fd) : _fd(fd), _phase(ClientSocket::RECV), _lastSendTimestamp(std::time(NULL)) {}

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
    this->_lastSendTimestamp = std::time(NULL);
    return ClientSocket::SEND;
}

ClientSocket::csphase ClientSocket::trySend() {
#if defined(_LINUX)
    const int flags = MSG_DONTWAIT | MSG_NOSIGNAL;
#elif defined(_DARWIN)
    const int flags = MSG_DONTWAIT | SO_NOSIGPIPE;
#else
    const int flags = MSG_DONTWAIT | MSG_NOSIGNAL;
#endif
    const char *msg = "HTTP/1.1 200 Ok\nContent-Length: 11\n\nHelloworld!";
    if ((this->_revents & POLLOUT) != POLLOUT) {
        return this->_phase;
    }

    if (send(this->_fd, msg, std::strlen(msg), flags) == -1) {
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

std::time_t ClientSocket::getLastSendTimestamp() const {
    return this->_lastSendTimestamp;
}

void ClientSocket::setLastSendTimestamp(std::time_t const lastSendTimestamp) {
    this->_lastSendTimestamp = lastSendTimestamp;
}
