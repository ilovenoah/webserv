#ifndef CLIENTSOCKET_HPP
#define CLIENTSOCKET_HPP

class ClientSocket {
    private:
        int _fd;
    public:
        ClientSocket();
        ClientSocket(int const fd);
        int getFd() const;
};

#endif