#include "Response.hpp"

Response::Response() : _httpVersion("HTTP/1.1"), _server(NULL), _location(NULL) {}

void Response::setServerPointer(Config &config, Request const &request, std::string const &ipAddr, std::string const &port) {
	std::string listen(ipAddr + ":" + port);
	Result<std::string, bool> res(request.getHeaderValue("Host"));
	if (res.isError() == true) {
		this->_server = config.getDefaultServer(listen).getOk();
	}
	else {
		std::string hostName(res.getOk());
		this->_server = config.getServerPointer(listen, hostName);
	}
}

void Response::setLocationPointer(const std::string &path) {
	if (this->_server == NULL) { return ; }
	this->_location = this->_server->getLocationPointer(path);
}
	(void)config;
	(void)request;
	this->_httpVersion = "HTTP/1.1";
	this->_status = "200";
	this->_statusMsg = "Ok";
	this->_headers.insert(
		std::pair<std::string, std::string>("Content-Length", "11"));
	this->_body = "Helloworld!";
	return ClientSocket::SEND;
}

std::string Response::getEntireData() const {
	std::string entireData;
	entireData += this->_httpVersion + " " + this->_status + " " +
				  this->_statusMsg + "\r\n";
	for (std::map<std::string, std::string>::const_iterator iter =
			 this->_headers.begin();
		 iter != this->_headers.end(); ++iter) {
		entireData += iter->first + ": " + iter->second + "\r\n";
	}
	entireData += "\r\n";
	entireData += this->_body;
	return entireData;
}