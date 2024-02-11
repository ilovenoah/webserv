#ifndef RESPONSE_HPP
#define RESPONSE_HPP
#include <map>
#include <string>

#include "ClientSocket.hpp"
#include "Config.hpp"
#include "Server.hpp"
#include "Location.hpp"
#include "Request.hpp"

class Response {
		// public:
		// 	enum rsphase {
		// 	};
	private:
		std::string _httpVersion;
		std::string _status;
		std::string _statusMsg;
		std::map<std::string, std::string, CaseInsensitiveCompare> _headers;
		std::string _body;
		Server *_server;
		Location *_location;

	public:
		Response();
		void setServerPointer(Config &config, Request const &request, std::string const &ipAddr, std::string const &port);
		void setLocationPointer(const std::string &path);
		void printConfigInfo() const;
		ClientSocket::csphase load(Config &config,  Request const &request);
		std::string getEntireData() const;
};

#endif