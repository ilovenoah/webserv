#include "loop.hpp"

// static std::string convertIPAddress(in_addr_t address) {
//     std::ostringstream oss;
//     oss << (address & 0xFF) << '.'               // 最下位のバイト
//         << ((address >> 8) & 0xFF) << '.'        // さらに次のバイト
//         << ((address >> 16) & 0xFF) << '.'       // 次のバイト
//         << ((address >> 24) & 0xFF);             // 最上位のバイト
//     return oss.str();
// }

static bool setRevents(std::map<int, ServerSocket> &ssmap, std::map<int, ClientSocket*> &csmap) {
    std::vector<struct pollfd> pollfds;
    for(std::map<int, ServerSocket>::iterator iter = ssmap.begin(); iter != ssmap.end(); ++iter) {
        struct pollfd pfd;
        std::memset(&pfd, 0, sizeof(struct pollfd));
        pfd.fd = iter->first;
        pfd.events = POLLIN;
        pollfds.push_back(pfd);
    }
    for(std::map<int, ClientSocket*>::iterator iter = csmap.begin(); iter != csmap.end(); ++iter) {
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
        else if (csmap.find(iter->fd) != csmap.end()) { csmap[iter->fd]->setRevents(iter->revents); }
    }
    return true;
}

static ClientSocket *createCsocket(std::pair<int, sockaddr_in> socketInfo) {
    return new(std::nothrow) ClientSocket(socketInfo.first);
}

static ClientSocket::csphase detectTimedOutClientSocket(ClientSocket &cs) {
    if (std::difftime(std::time(NULL), cs.getLastSendTimestamp()) > 5) {
        return ClientSocket::CLOSE;
    }
    if ((cs.getPhase() & POLLHUP) == POLLHUP) { return ClientSocket::CLOSE; }
    return cs.getPhase();
}

bool loop(std::map<int, ServerSocket> &ssmap, Config const &config) {
    std::map<int, ClientSocket*> csmap;
    std::map<int, Request> rqmap;
	std::map<int, Response> rsmap;
    while(true) {
        if (setRevents(ssmap, csmap) == false) { return false; }
        for(std::map<int, ServerSocket>::iterator iter = ssmap.begin(); iter != ssmap.end(); ++iter) {
            std::pair<int, sockaddr_in> socketInfo = iter->second.tryAccept();
            if (socketInfo.first == -1) { continue; }
            ClientSocket *newCs;
            newCs = createCsocket(socketInfo);
            if (newCs == NULL) {
                utils::putSysError("new");
                close(socketInfo.first);
                continue;
            }
            csmap.insert(std::pair<int, ClientSocket*>(socketInfo.first, newCs));
            rqmap.insert(std::pair<int, Request>(socketInfo.first, Request()));
        }
        for (std::map<int, ClientSocket*>::iterator iter = csmap.begin(); iter != csmap.end();) {
            iter->second->setPhase(detectTimedOutClientSocket(*(iter->second)));
            switch (iter->second->getPhase()) {
                case ClientSocket::RECV:
                    iter->second->setPhase(iter->second->tryRecv());
                    ++iter;
                    break;
                case ClientSocket::SEND:
#if defined(_DEBUG)
					std::clog << rqmap[iter->first].getEntireData() << std::endl;
#endif
                    iter->second->setPhase(iter->second->trySend());
                    ++iter;
                    break;
                case ClientSocket::CLOSE:
                    std::map<int, ClientSocket*>::iterator toErase = iter;
                    ++iter;
                    toErase->second->close();
                    delete toErase->second;
                    rqmap.erase(toErase->first);
                    csmap.erase(toErase);
#if defined(_DEBUG)
					std::clog << "ClientSocket size: " << csmap.size() << std::endl;
					std::clog << "Request size: " << rqmap.size() << std::endl;
#endif
                    break;
            }
        }
        for (std::map<int, Request>::iterator iter = rqmap.begin(); iter != rqmap.end(); ++iter) {
            std::map<int, ClientSocket*>::iterator csiter = csmap.find(iter->first);
            if (csiter != csmap.end() && (csiter->second->buffer.str().find("\r\n") != std::string::npos ||  iter->second.getReqphase() == Request::RQBODY)) {
                ClientSocket::csphase nextcsphase = iter->second.load(csiter->second->buffer);
                csiter->second->setPhase(nextcsphase);
            }
            std::map<int, Response>::iterator rsiter = rsmap.find(iter->first);
			if (rsiter == rsmap.end() && iter->second.getReqphase() == Request::RQFIN) {
				rsmap.insert(std::pair<int, Response>(iter->first, Response()));
			}
        }
        for (std::map<int, Response>::iterator iter = rsmap.begin(); iter != rsmap.end(); ++iter) {
			std::map<int, Request>::iterator rqiter = rqmap.find(iter->first);
			if (rqiter != rqmap.end()) {
				iter->second.load(config, rqiter->second);
			}
		}
		
    }
    return true;
}
