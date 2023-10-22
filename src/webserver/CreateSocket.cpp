#include "CreateSocket.hpp"

int setNonBlocking(int fd) {
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1)
		return -1;
	return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int getSocket(struct addrinfo *res) {
	int sockfd;
	try {
		sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (sockfd == -1) {
			throw GenericException(SOCKET_ERROR);
		}
		if (setNonBlocking(sockfd) == -1) {
			throw GenericException(NONBLOCK_ERROR);
		}
		return sockfd;
	} catch (GenericException &e) {
		std::cerr << e.what() << std::endl;
		if (sockfd != -1) {
			close(sockfd);	// Close the socket if it's opened
		}
		freeaddrinfo(res);
		std::exit(EXIT_FAILURE);
	}
}

void setSocketOption(int sockfd, struct addrinfo *res) {
	int optval = 1;
	int no = 0;
	try {
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
			throw GenericException(FAIL_SOCKOPT);
		}
		if (res->ai_family == AF_INET6) {
			if (setsockopt(sockfd, IPPROTO_IPV6, IPV6_V6ONLY, (void *)&no, sizeof(no)) == -1) {
				throw GenericException(FAIL_SOCKOPT);
			}
		}
	} catch (GenericException &e) {
		std::cerr << e.what() << std::endl;
		close(sockfd);
		freeaddrinfo(res);
		std::exit(EXIT_FAILURE);
	}
}

std::vector<int> getSockets(const std::vector<Server> &servervec) {
	std::vector<int> sockets;
	struct addrinfo hints, *res;
	int sockfd;

	try {
		for (size_t i = 0; i < servervec.size(); i++) {
			std::memset(&hints, 0, sizeof hints);
			hints.ai_family = AF_UNSPEC;
			hints.ai_socktype = SOCK_STREAM;
			hints.ai_flags = AI_PASSIVE;

			if (getaddrinfo(servervec[i].getIpAddress().c_str(), servervec[i].getPort().c_str(), &hints, &res) != 0) {
				throw GenericException(FAIL_GET_ADDR);
			}
			sockfd = getSocket(res);
			setSocketOption(sockfd, res);
			if (bind(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
				throw GenericException(BIND_ERROR + servervec[i].getListen());
			}
			if (listen(sockfd, MAXLEN) == -1) {
				throw GenericException(FAIL_LISTEN);
			}
			freeaddrinfo(res);
			sockets.push_back(sockfd);
		}
	} catch (GenericException &e) {
		std::cerr << e.what() << std::endl;
		close(sockfd);
		freeaddrinfo(res);
		std::exit(EXIT_FAILURE);
	}
	return sockets;
}
