#include "ServerLoop.hpp"

int initilizeEpollfd(std::vector<int> sockets) {
	int epollfd = 0;
	try {
		epollfd = epoll_create1(0);
		if (epollfd == -1) {
			throw GenericException(EPOLL_CREATE_ERROR);
		}
		for (size_t i = 0; i < sockets.size(); i++) {
			struct epoll_event event;
			event.data.fd = sockets[i];
			event.events = EPOLLIN;
			if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sockets[i], &event) == -1) {
				throw GenericException(EPOLL_CTL_ERROR);
			}
		}
	} catch (GenericException &e) {
		std::cerr << e.what() << std::endl;
		std::exit(EXIT_FAILURE);
	}
	return epollfd;
}

int startServerLoop(Config &conf_info) {
	std::vector<int> sockets = getSockets(conf_info.getServers());
	int epollfd = initilizeEpollfd(sockets);
	struct epoll_event events[MAXEVENTS];  // 一度に処理する最大のイベント数
	while (1) {
		int n = epoll_wait(epollfd, events, MAXEVENTS, -1);
		for (int i = 0; i < n; i++) {
			if (std::find(sockets.begin(), sockets.end(), events[i].data.fd) != sockets.end()) {
				struct sockaddr_in clientAddr;
				socklen_t clientAddrSize = sizeof(clientAddr);
				int clientSock = accept(events[i].data.fd, (struct sockaddr *)&clientAddr, &clientAddrSize);
				if (clientSock == -1) {
					perror("accept failed");
					continue;
				}
				if (setNonBlocking(clientSock) == -1) {	 // ここでノンブロッキングに設定
					perror("Setting non-blocking failed for clientSock");
					close(clientSock);
					continue;
				}
				struct epoll_event clientEvent;
				clientEvent.data.fd = clientSock;
				clientEvent.events = EPOLLIN;  // 初めは読み込みのみを監視
				if (epoll_ctl(epollfd, EPOLL_CTL_ADD, clientSock, &clientEvent) == -1) {
					perror("epoll_ctl failed for clientSock");
					close(clientSock);
				}
				continue;  // このiterationはここで終了
			}
			if (events[i].events & EPOLLIN) {
				char buffer[BUFFER_SIZE];
				ssize_t bytesRead = read(events[i].data.fd, buffer, sizeof(buffer));
				if (bytesRead < 0) {
					// これはエラーを示しています
					perror("Read error");
					close(events[i].data.fd);
					continue;
				}
				buffer[bytesRead] = '\0';
				std::cout << buffer << std::endl;
				struct epoll_event event;
				event.data.fd = events[i].data.fd;
				event.events = EPOLLIN | EPOLLOUT;
				epoll_ctl(epollfd, EPOLL_CTL_MOD, events[i].data.fd, &event);
			}
			if (events[i].events & EPOLLOUT) {
				// データを書き込む処理

				// 書き込みが完了したら、EPOLLOUT の監視を停止します
				struct epoll_event event;
				event.data.fd = events[i].data.fd;
				event.events = EPOLLIN;
				epoll_ctl(epollfd, EPOLL_CTL_MOD, events[i].data.fd, &event);
			}
		}
	}
}
