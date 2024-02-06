#include "Server.hpp"

const std::string &Server::getServername() const {
	return this->_servername;
}

void Server::setServername(std::string const &servername) {
	this->_servername = servername;
}

const std::string &Server::getIpaddr() const {
	return this->_ipAddr;
}

void Server::setIpaddr(std::string const &ipaddr) {
	this->_ipAddr = ipaddr;
}

const std::string &Server::getPort() const {
	return this->_port;
}

void Server::setPort(std::string const &port) {
	this->_port = port;
}

const std::string &Server::getAllowMethods() const {
	return this->_allowMethods;
}

void Server::setAllowMethods(std::string const &allowMethods) {
	this->_allowMethods = allowMethods;
}

const bool &Server::getAutoindex() const {
	return this->_autoindex;
}

void Server::setAutoindex(bool const &autoindex) {
	this->_autoindex = autoindex;
}

const std::string &Server::getIndex() const {
	return this->_index;
}

void Server::setIndex(std::string const &index) {
	this->_index = index;
}

const std::size_t &Server::getClientMaxBodySize() const {
	return this->_clientMaxBodySize;
}

void Server::setClientMaxBodySize(std::size_t const &clientMaxBodySize) {
	this->_clientMaxBodySize = clientMaxBodySize;
}

const std::map<std::string, Location> &Server::getLocations() const {
	return this->_locations;
}

const std::map<std::string, std::string> &Server::getReturns() const {
	return this->_returns;
}