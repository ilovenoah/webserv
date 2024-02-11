#ifndef CLIENTSOCKET_HPP
#define CLIENTSOCKET_HPP

#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <ctime>

#include "ServerSocket.hpp"
#include "utils.hpp"

#define BUFFERSIZE 4096

class ClientSocket {
	public:
		enum csphase { RECV, SEND, CLOSE };

	private:
		int _fd;
		short _revents;
		ClientSocket::csphase _phase;
		std::time_t _lastSendTimestamp;
		ServerSocket *_serverSocket;

	public:
		std::stringstream buffer;
		ClientSocket();
		ClientSocket(int const fd, ServerSocket *serverSocket);
		int getFd() const;
		void setRevents(short revents);
		short getRevents() const;
		ClientSocket::csphase tryRecv();
		ClientSocket::csphase trySend(std::string const &msg);
		void close();
		void setPhase(ClientSocket::csphase const phase);
		ClientSocket::csphase getPhase() const;
		std::time_t getLastSendTimestamp() const;
		void setLastSendTimestamp(std::time_t const lastSendTimestamp);
		bool findCRLF();
		ServerSocket *getServerSocket() const;
};

#endif
