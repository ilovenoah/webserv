#include "loop.hpp"

// static std::string convertIPAddress(in_addr_t address) {
//     std::ostringstream oss;
//     oss << (address & 0xFF) << '.'               // 最下位のバイト
//         << ((address >> 8) & 0xFF) << '.'        // さらに次のバイト
//         << ((address >> 16) & 0xFF) << '.'       // 次のバイト
//         << ((address >> 24) & 0xFF);             // 最上位のバイト
//     return oss.str();
// }

static bool setRevents(std::map<int, ServerSocket> &ssmap) {
    std::vector<struct pollfd> pollfds;
    for(std::map<int, ServerSocket>::iterator iter = ssmap.begin(); iter != ssmap.end(); ++iter) {
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
        ssmap[iter->fd].setRevents(iter->revents);
    }
    return true;
}

bool loop(std::map<int, ServerSocket> &ssmap) {
    while(true) {
        if (setRevents(ssmap) == false) {
            return false;
        }
        for(std::map<int, ServerSocket>::iterator iter = ssmap.begin(); iter != ssmap.end(); ++iter) {
            std::pair<int, sockaddr_in> socketInfo = iter->second.tryAccept();
            (void)socketInfo;
        }
    }
}