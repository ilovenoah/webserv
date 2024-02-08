#include "Server.hpp"

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

bool Server::setServername(std::string const &attribute, std::fstream &file) {
	(void)file;
	std::stringstream ss(attribute);
	std::string elem;
	ss >> elem;
	if (ss.eof() == true) { return false; }
	elem.clear();
	ss >> elem;
	if (ss.eof() == false) { return false; }
	this->_servername = elem;
	return true;
}

static bool isPortOutOfRange(std::string &port) {
	std::size_t portNum(utils::decStrToSizeT(port));

	return MIN_USERPORT > portNum || portNum > MAX_USERPORT;
}

bool Server::setListen(std::string const &attribute, std::fstream &file) {
	(void)file;
	std::stringstream ss(attribute);
	std::string elem;
	std::string ipAddr;
	std::string port;
	ss >> elem;
	if (ss.eof() == true) { return false; }
	elem.clear();
	ss >> elem;
	if (ss.eof() == false) { return false; }
	ss.str("");
	ss.clear();
	ss << elem;
	std::getline(ss, ipAddr, ':');
	if (ss.eof() == true) {
		this->_port = ipAddr;
		if (isPortOutOfRange(port) == true) { return false; }
		return true;
	}
	if (isPortOutOfRange(port) == true) { return false; }
	std::getline(ss, port);
	this->_ipAddr = ipAddr;
	this->_port = port;
	return true;
}

std::string Server::getListen() const {
	return this->_ipAddr + ':' + this->_port;
}

const std::string &Server::getIpaddr() const {
	return this->_ipAddr;
}

const std::string &Server::getPort() const {
	return this->_port;
}

bool Server::setRoot(std::string const &attribute, std::fstream &file) {
	(void)file;
	std::stringstream ss(attribute);
	std::string elem;
	ss >> elem;
	if (ss.eof() == true) { return false; }
	elem.clear();
	ss >> elem;
	if (ss.eof() == false) { return false; }
	this->_root = elem;
	return true;
}

const std::string &Server::getRoot() const {
	return this->_root;
}

bool Server::setAllowMethods(std::string const &attribute, std::fstream &file) {
	(void)file;
	std::stringstream ss(attribute);
	std::string elem;
	ss >> elem;
	if (ss.eof() == true) { return false; }
	while (ss.eof() == false) {
		elem.clear();
		ss >> elem;
		if (elem != "GET" && elem != "POST" && elem != "DELETE") { return false; }
		this->_allowMethods.push_back(elem);
	}
	return true;
}

const std::vector<std::string> &Server::getAllowMethods() const {
	return this->_allowMethods;
}
bool Server::setAutoindex(std::string const &attribute, std::fstream &file) {
	(void)file;
	std::stringstream ss(attribute);
	std::string elem;
	ss >> elem;
	if (ss.eof() == true) { return false; }
	elem.clear();
	ss >> elem;
	if (ss.eof() == false) { return false; }
	if (elem == "on") {
		this->_autoindex = true;
	} else if (elem == "off") {
		this->_autoindex = false;
	} else {
		return false;
	}
	return true;
}

const bool &Server::getAutoindex() const {
	return this->_autoindex;
}

bool Server::setIndex(std::string const &attribute, std::fstream &file) {
	(void)file;
	std::stringstream ss(attribute);
	std::string elem;
	ss >> elem;
	if (ss.eof() == true) { return false; }
	while (ss.eof() == false) {
		elem.clear();
		ss >> elem;
		this->_index.push_back(elem);
	}
	return true;
}

const std::vector<std::string> &Server::getIndex() const {
	return this->_index;
}

bool Server::setClientMaxBodySize(std::string const &attribute, std::fstream &file) {
	(void)file;
	std::stringstream ss(attribute);
	std::string elem;
	ss >> elem;
	if (ss.eof() == true) { return false; }
	elem.clear();
	ss >> elem;
	if (ss.eof() == false) { return false; }
	this->_clientMaxBodySize = utils::decStrToSizeT(elem);
	return true;
}

const std::size_t &Server::getClientMaxBodySize() const {
	return this->_clientMaxBodySize;
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