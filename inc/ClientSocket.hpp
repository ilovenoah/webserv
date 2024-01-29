#ifndef CLIENTSOCKET_HPP
#define CLIENTSOCKET_HPP

class ClientSocket {
    private:
        int _fd;
        short _revents;
    public:
        ClientSocket();
        ClientSocket(int const fd);
        int getFd() const;
        void setRevents(short revents);
};

#endif