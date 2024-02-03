#ifndef RESPONSE_HPP
#define RESPONSE_HPP
#include <string>
#include <map>
#include "ClientSocket.hpp"
#include "Config.hpp"
#include "Request.hpp"

class Response {
	// public:
	// 	enum rsphase {
	// 	};
	private:
		std::string _httpVersion;
		std::string _status;
		std::string _statusMsg;
		std::map<std::string, std::string> _headers;
		std::string _body;

	public:
		Response();
		ClientSocket::csphase load(Config const &config, Request const &request);
		std::string getEntireData() const;
};

#endif