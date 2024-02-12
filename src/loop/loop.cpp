#include "loop.hpp"

static bool setRevents(std::map<int, ServerSocket> &ssmap,
					   std::map<int, ClientSocket *> &csmap) {
	std::vector<struct pollfd> pollfds;
	for (std::map<int, ServerSocket>::iterator iter = ssmap.begin();
		 iter != ssmap.end(); ++iter) {
		struct pollfd pfd;
		std::memset(&pfd, 0, sizeof(struct pollfd));
		pfd.fd = iter->first;
		pfd.events = POLLIN;
		pollfds.push_back(pfd);
	}
	for (std::map<int, ClientSocket *>::iterator iter = csmap.begin();
		 iter != csmap.end(); ++iter) {
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
	for (std::vector<struct pollfd>::iterator iter = pollfds.begin();
		 iter != pollfds.end(); ++iter) {
		if (ssmap.find(iter->fd) != ssmap.end()) {
			ssmap[iter->fd].setRevents(iter->revents);
		} else if (csmap.find(iter->fd) != csmap.end()) {
			csmap[iter->fd]->setRevents(iter->revents);
		}
	}
	return true;
}

static ClientSocket *createCsocket(std::pair<int, sockaddr_in> socketInfo, ServerSocket *serverSocket) {
	return new (std::nothrow) ClientSocket(socketInfo.first, serverSocket);
}

static ClientSocket::csphase detectTimedOutClientSocket(ClientSocket &cs) {
	if (std::difftime(std::time(NULL), cs.getLastSendTimestamp()) > 5) {
		return ClientSocket::CLOSE;
	}
	if ((cs.getPhase() & POLLHUP) == POLLHUP) {
		return ClientSocket::CLOSE;
	}
	return cs.getPhase();
}

bool loop(std::map<int, ServerSocket> &ssmap, Config &config) {
	std::map<int, ClientSocket *> csmap;
	std::map<int, Request> rqmap;
	std::map<int, Response> rsmap;
	while (true) {
		if (setRevents(ssmap, csmap) == false) {
			return false;
		}
		for (std::map<int, ServerSocket>::iterator iter = ssmap.begin();
			 iter != ssmap.end(); ++iter) {
			std::pair<int, sockaddr_in> socketInfo = iter->second.tryAccept();
			if (socketInfo.first == -1) {
				continue;
			}
			ClientSocket *newCs;
			newCs = createCsocket(socketInfo, &(iter->second));
			if (newCs == NULL) {
				utils::putSysError("new");
				close(socketInfo.first);
				continue;
			}
			csmap.insert(
				std::pair<int, ClientSocket *>(socketInfo.first, newCs));
			rqmap.insert(std::pair<int, Request>(socketInfo.first, Request()));
		}
		for (std::map<int, ClientSocket *>::iterator iter = csmap.begin();
			 iter != csmap.end();) {
			iter->second->setPhase(detectTimedOutClientSocket(*(iter->second)));
			switch (iter->second->getPhase()) {
				case ClientSocket::RECV: {
					iter->second->setPhase(iter->second->tryRecv());
					++iter;
					break;
				}
				case ClientSocket::SEND: {
#if defined(_DEBUG)
					std::clog << rqmap[iter->first].getEntireData()
							  << std::endl;
#endif
					std::map<int, Response>::iterator rsiter =
						rsmap.find(iter->first);
					std::map<int, Request>::iterator rqiter =
						rqmap.find(iter->first);
					if (rsiter != rsmap.end() && rqiter != rqmap.end()) {
						iter->second->setPhase(iter->second->trySend(
							rsiter->second.getEntireData()));
						rsmap.erase(rsiter);
						rqiter->second.init();
					}
					++iter;
					break;
				}
				case ClientSocket::CLOSE: {
					std::map<int, ClientSocket *>::iterator toErase = iter;
					std::map<int, Request>::iterator rqiter =
						rqmap.find(toErase->first);
					if (rqiter != rqmap.end()) {
						rqmap.erase(rqiter);
					}
					++iter;
					toErase->second->close();
					delete toErase->second;
					csmap.erase(toErase);
#if defined(_DEBUG)
					std::clog << "ClientSocket size: " << csmap.size()
							  << std::endl;
					std::clog << "Request size: " << rqmap.size() << std::endl;
					std::clog << "Response size: " << rsmap.size() << std::endl;
					int checkFd = open("/dev/null", O_RDONLY);
					std::clog << "check fd: " << checkFd << std::endl;
					close(checkFd);
#endif
					break;
				}
			}
		}
		for (std::map<int, Request>::iterator iter = rqmap.begin();
			 iter != rqmap.end(); ++iter) {
			std::map<int, ClientSocket *>::iterator csiter =
				csmap.find(iter->first);
			if (csiter != csmap.end() &&
				(utils::findCRLF(csiter->second->buffer) ||
				 iter->second.getReqphase() == Request::RQBODY)) {
				ClientSocket::csphase nextcsphase =
					iter->second.load(csiter->second->buffer);
				csiter->second->setPhase(nextcsphase);
			}
			std::map<int, Response>::iterator rsiter = rsmap.find(iter->first);
			if (rsiter == rsmap.end() &&
				iter->second.getReqphase() == Request::RQFIN) {
				rsmap.insert(std::pair<int, Response>(iter->first, Response()));
				std::map<int, Response>::iterator rsiter = rsmap.find(iter->first);
				std::map<int, Request>::iterator rqiter = rqmap.find(iter->first);
				std::map<int, ClientSocket *>::iterator csiter = csmap.find(iter->first);
				rsiter->second.setServerPointer(config, rqiter->second, csiter->second->getServerSocket()->getIpaddress(),  csiter->second->getServerSocket()->getPort());
				rsiter->second.setLocationPointer(rqiter->second.getPath());
#if defined(_DEBUG)
rsiter->second.printConfigInfo();
#endif
			}
		}
		for (std::map<int, Response>::iterator iter = rsmap.begin();
			 iter != rsmap.end(); ++iter) {
			std::map<int, Request>::iterator rqiter = rqmap.find(iter->first);
			std::map<int, ClientSocket *>::iterator csiter =
				csmap.find(iter->first);
			if (rqiter != rqmap.end() && csiter != csmap.end()) {
				ClientSocket::csphase nextcsphase =
					iter->second.load(config, rqiter->second);
				csiter->second->setPhase(nextcsphase);
			}
		}
	}
	return true;
}
