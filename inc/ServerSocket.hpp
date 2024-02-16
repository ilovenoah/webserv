#ifndef SERVERSOCKET_HPP
#define SERVERSOCKET_HPP

#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <sstream>
#include <string>
#include <utility>

#include "utils.hpp"

#define LISTEN_BACKLOG 1000

class ServerSocket {
	private:
		int _fd;
		std::string _ipaddress;
		std::string _port;
		short _revents;

	public:
		ServerSocket();
		ServerSocket(std::string ipaddress, std::string port);
		bool init();
		void setRevents(short revents);
		std::pair<int, struct sockaddr_in> tryAccept();
		int getFd() const;
		const std::string &getIpaddress() const;
		const std::string &getPort() const;
};

#endif