#include "Server.hpp"

static std::map<std::string, void (Server::*)(const std::string&, std::fstream&)> initSetterMap() {
	std::map<std::string, void (Server::*)(const std::string&, std::fstream&)> srvSetterMap;
	srvSetterMap["server_name"] = &Server::setServerName;
	srvSetterMap["listen"] = &Server::setListen;
	srvSetterMap["root"] = &Server::setRoot;
	srvSetterMap["allow_methods"] = &Server::setAllowMethods;
	srvSetterMap["autoindex"] = &Server::setAutoIndex;
	srvSetterMap["index"] = &Server::setIndex;
	srvSetterMap["client_body_limit"] = &Server::setClientBodyLimit;
	srvSetterMap["cgi_info"] = &Server::setCgiInfo;
	srvSetterMap["return"] = &Server::setReturn;
	srvSetterMap["location"] = &Server::setLocation;
	srvSetterMap["error_page"] = &Server::setErrorPage;
}

std::map<std::string, void (Server::*)(const std::string&, std::fstream&)> Server::_setterMap = initSetterMap();

static std::vector<std::string> initAllowedMethods() {
    std::vector<std::string> methods;
    methods.push_back("GET");
    methods.push_back("POST");
    methods.push_back("DELETE");
    return methods;
}

Server::Server() : _servername(""), _ipAddr("0.0.0.0"), _port("8000"), _allowMethods(initAllowedMethods()), _autoindex(false), _clientMaxBodySize(ONEMEGA) {}

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

const std::vector<std::string> &Server::getAllowMethods() const {
	return this->_allowMethods;
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

const std::map<std::string, std::string> &Server::getErrorPages() const {
	return this->_errorPages;
}