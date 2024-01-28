#ifndef SERVERSOCKET_HPP
#define SERVERSOCKET_HPP

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sstream>
#include <sys/types.h>
#include <fcntl.h>
#include "utils.hpp"


#define LISTEN_BACKLOG 1000

class ServerSocket {
    private:
        int _fd;
        std::string _ipaddress;
        std::string _port;
        short _revents;
        ServerSocket();
        
    public:
        ServerSocket(std::string ipaddress, std::string port);
        bool init();
        void setRevents(short revents);

};


#endif 