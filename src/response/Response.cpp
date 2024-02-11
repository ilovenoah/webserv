#include "Response.hpp"

Response::Response() : _httpVersion("HTTP/1.1") {}

ClientSocket::csphase Response::load(Config const &config,
									 Request const &request, std::string const &ipAddr, std::string const &port) {
	Server server;
	std::string listen(ipAddr + ":" + port);
	Result<std::string, bool> res(request.getHeaderValue("Host"));
	if (res.isError() == true) {
		server = *(config.getDefaultServer(listen).getOk());
	}
	else {
		std::string hostName(res.getOk());
		std::map<std::string, std::map<std::string, Server> >::const_iterator ipiter = config.getServers().find(listen);
		std::map<std::string, Server>::const_iterator sviter = ipiter->second.find(hostName);

		if (sviter == ipiter->second.end()) { server = *(config.getDefaultServer(listen).getOk()); }
		else { server = sviter->second; }
	}
	{
		std::clog << "<< Routing result >>" << std::endl;
		std::clog << "Server name: " << server.getServername() << std::endl;
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