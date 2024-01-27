#include "ServerSocket.hpp"

ServerSocket::ServerSocket() {}
ServerSocket::ServerSocket(std::string ipaddress, std::string port) : _ipaddress(ipaddress), _port(port) {}