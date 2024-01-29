#include "loop.hpp"

// static std::string convertIPAddress(in_addr_t address) {
//     std::ostringstream oss;
//     oss << (address & 0xFF) << '.'               // 最下位のバイト
//         << ((address >> 8) & 0xFF) << '.'        // さらに次のバイト
//         << ((address >> 16) & 0xFF) << '.'       // 次のバイト
//         << ((address >> 24) & 0xFF);             // 最上位のバイト
//     return oss.str();
// }

static bool setRevents(std::map<int, ServerSocket> &ssmap, std::map<int, ClientSocket> &csmap) {
    std::vector<struct pollfd> pollfds;
    for(std::map<int, ServerSocket>::iterator iter = ssmap.begin(); iter != ssmap.end(); ++iter) {
        struct pollfd pfd;
        std::memset(&pfd, 0, sizeof(struct pollfd));
        pfd.fd = iter->first;
        pfd.events = POLLIN | POLLHUP;
        pollfds.push_back(pfd);
    }
    for(std::map<int, ClientSocket>::iterator iter = csmap.begin(); iter != csmap.end(); ++iter) {
        struct pollfd pfd;
        std::memset(&pfd, 0, sizeof(struct pollfd));
        pfd.fd = iter->first;
        pfd.events = POLLIN | POLLOUT | POLLHUP;
        pollfds.push_back(pfd);
    }
    if (poll(pollfds.data(), pollfds.size(), 0) == -1) {
        utils::putSysError("poll");
        return false;
    }
    for(std::vector<struct pollfd>::iterator iter = pollfds.begin(); iter != pollfds.end(); ++iter) {
        if (ssmap.find(iter->fd) != ssmap.end()) { ssmap[iter->fd].setRevents(iter->revents); }
        else if (csmap.find(iter->fd) != csmap.end()) { csmap[iter->fd].setRevents(iter->revents); }
    }
    return true;
}

static void checkClosedAndTimedOutClientSockets(std::map<int, ClientSocket> &csmap) {
    for (std::map<int, ClientSocket>::iterator iter = csmap.begin(); iter != csmap.end(); ++iter) {
        if ((iter->second.getRevents() & POLLHUP) == POLLHUP) {iter->second.setPhase(ClientSocket::CLOSE); }
        else if (std::difftime(std::time(NULL), iter->second.getLastSendTimestamp()) > 5) {
            close(iter->second.getFd());
            iter->second.setPhase(ClientSocket::CLOSE);
        }
    }
}

static ClientSocket createCsocket(std::pair<int, sockaddr_in> socketInfo) {
    ClientSocket cs(socketInfo.first);
    return cs;    
}

bool loop(std::map<int, ServerSocket> &ssmap) {
    std::map<int, ClientSocket> csmap;
    while(true) {
        if (setRevents(ssmap, csmap) == false) { return false; }
        for(std::map<int, ServerSocket>::iterator iter = ssmap.begin(); iter != ssmap.end(); ++iter) {
            std::pair<int, sockaddr_in> socketInfo = iter->second.tryAccept();
            if (socketInfo.first == -1) { continue; }
            csmap.insert(std::pair<int, ClientSocket>(socketInfo.first, createCsocket(socketInfo)));
        }
        checkClosedAndTimedOutClientSockets(csmap);
        for (std::map<int, ClientSocket>::iterator iter = csmap.begin(); iter != csmap.end();) {
            switch (iter->second.getPhase()) {
                case ClientSocket::RECV:
                    iter->second.setPhase(iter->second.tryRecv());
                    ++iter;
                    break;
                case ClientSocket::SEND:
                    iter->second.setPhase(iter->second.trySend());
                    ++iter;
                    break;
                case ClientSocket::CLOSE:
                    std::map<int, ClientSocket>::iterator toErase = iter;
                    ++iter;
                    csmap.erase(toErase);
                    break;
            }
        }
    }
    return true;
}