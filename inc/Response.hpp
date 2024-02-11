#ifndef RESPONSE_HPP
#define RESPONSE_HPP
#include <map>
#include <string>

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
		std::map<std::string, std::string, CaseInsensitiveCompare> _headers;
		std::string _body;

	public:
		Response();
		ClientSocket::csphase load(Config const &config,
								   Request const &request, std::string const &ipAddr, std::string const &port);
		std::string getEntireData() const;
};

#endif