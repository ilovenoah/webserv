#include "CreateSocket.hpp"

#include "Config.hpp"
#include "main.hpp"
#define MAXLEN 10

int setNonBlocking(int fd) {
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1)
		return -1;
	return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int getSocket(const Server &server, struct addrinfo *res) {
	int sockfd;
	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (sockfd == -1) {
		std::cerr << "socket creation failed." << std::endl;
		freeaddrinfo(res);
		std::exit(EXIT_FAILURE);
	}
	if (setNonBlocking(sockfd) == -1) {
		std::cerr << "Setting non-blocking failed." << std::endl;
		close(sockfd);	// 不要なソケットを閉じる
		freeaddrinfo(res);
		std::exit(EXIT_FAILURE);
	}
	return sockfd;
}

void setSocketOption(int sockfd, struct addrinfo *res) {
	int optval = 1;
	int no = 0;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
		std::cerr << "setsockopt failed." << std::endl;
		close(sockfd);
		freeaddrinfo(res);
		std::exit(EXIT_FAILURE);
	}
	if (res->ai_family == AF_INET6) {
		if (setsockopt(sockfd, IPPROTO_IPV6, IPV6_V6ONLY, (void *)&no, sizeof(no)) == -1) {
			std::cerr << "setsockopt failed." << std::endl;
			close(sockfd);
			freeaddrinfo(res);
			std::exit(EXIT_FAILURE);
		}
	}
}

std::vector<int> getSockets(const std::vector<Server> &servervec) {
	std::vector<int> sockets;
	struct addrinfo hints, *res;
	int sockfd;

	for (size_t i = 0; i < servervec.size(); i++) {
		std::memset(&hints, 0, sizeof hints);
		hints.ai_family = AF_UNSPEC;	  // IPv4もしくはIPv6
		hints.ai_socktype = SOCK_STREAM;  // TCP
		hints.ai_flags = AI_PASSIVE;	  // サーバーソケット用

		if (getaddrinfo(servervec[i].getIpAddress().c_str(), servervec[i].getPort().c_str(), &hints, &res) != 0) {
			std::cerr << "getaddrinfo failed." << std::endl;
			continue;  // toerrorhondol
		}
		sockfd = getSocket(servervec[i], res);
		setSocketOption(sockfd, res);
		if (bind(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
			std::cerr << "Bind failed for " << servervec[i].getIpAddress().c_str() << ":" << servervec[i].getPort()
					  << ". Error: " << strerror(errno) << std::endl;
			close(sockfd);		// ソケットを閉じる
			freeaddrinfo(res);	// メモリを解放
			std::exit(EXIT_FAILURE);
		}
		if (listen(sockfd, MAXLEN) == -1) {	 // 10は待ち行列の最大数
			std::cerr << "listen failed." << std::endl;
			freeaddrinfo(res);	// メモリを解放
			std::exit(EXIT_FAILURE);
		}
		freeaddrinfo(res);
		sockets.push_back(sockfd);
	}
	return sockets;
}
