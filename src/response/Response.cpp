#include "Response.hpp"

Response::Response() : _httpVersion("HTTP/1.1") {}

ClientSocket::csphase Response::load(Config const &config, Request const &request) {
	(void)config;
	(void)request;
	this->_httpVersion = "HTTP/1.1";
	this->_status = "200";
	this->_statusMsg = "Ok";
	this->_headers.insert(std::pair<std::string, std::string>("Content-Length", "11"));
	this->_body = "Helloworld!";
	return ClientSocket::SEND;
}