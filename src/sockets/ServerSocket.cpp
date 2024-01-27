#include "ServerSocket.hpp"

ServerSocket::ServerSocket() {}
ServerSocket::ServerSocket(std::string ipaddress, std::string port) : _ipaddress(ipaddress), _port(port) {}

static u_int32_t convertIpStrToUint(std::string const &ipaddr) {
    u_int32_t s_addr(0);
    std::stringstream ss(ipaddr);
    int byte(0);
    int bitshift(0);
    char dot;

    while (ss.eof() == false) {
        ss >> byte;
        ss >> dot;
        s_addr += byte << bitshift;
        bitshift += 8;
    }
    return s_addr;
}

bool ServerSocket::init() {
    struct sockaddr_in s_addr;
    std::stringstream ss(_port);
    unsigned int port;
    ss >> port;
	int optval = 1;
    
    std::memset(&s_addr, 0, sizeof(sockaddr_in));
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(port);
    s_addr.sin_addr.s_addr = convertIpStrToUint(_ipaddress);
    _fd = socket(s_addr.sin_family, SOCK_STREAM, 0);
    if (_fd == -1) {
        perror("socket");
        return false;
    }
    if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
        perror("setsockopt");
        return false;
    }
    if (bind(_fd, (const struct sockaddr *)&s_addr, sizeof(s_addr)) == -1) {
        perror("bind");
        return false;
    }
    if (listen(_fd, LISTEN_BACKLOG) == -1) {
        perror("listen");
        return false;
    }
    if (fcntl(_fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1) {
        perror("fcntl");
        return false;
    }
    return true;
}