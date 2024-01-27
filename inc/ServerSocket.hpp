#ifndef SERVERSOCKET_HPP
#define SERVERSOCKET_HPP

#include <string>

class ServerSocket {
    private:
        int _fd;
        std::string _ipaddress;
        std::string _port;
        ServerSocket();
        
    public:
        ServerSocket(std::string ipaddress, std::string port);

};


#endif 