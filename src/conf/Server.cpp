#include "Server.hpp"

std::map<std::string, bool (Location::*)(std::string const&, std::fstream&)> Server::initSetterMap() {
	std::map<std::string, bool (Location::*)(const std::string&, std::fstream&)> srvSetterMap;
	srvSetterMap["allow_methods"] = &Location::setAllowMethods;
	srvSetterMap["autoindex"] = &Location::setAutoIndex;
	srvSetterMap["index"] = &Location::setIndex;
	srvSetterMap["client_body_limit"] = &Location::setClientMaxBodySize;
	srvSetterMap["cgi_extensions"] = &Location::setCgiExtensions;
	srvSetterMap["return"] = &Location::setReturn;
	srvSetterMap["error_page"] = &Location::setErrorPages;
	return srvSetterMap;
}

std::map<std::string, bool (Location::*)(std::string const&, std::fstream&)> Server::_setterMap = initSetterMap();

Server::Server() : AConfigurable(), _servername(""), _ipAddr("0.0.0.0"), _port("8000") {}

const std::string &Server::getServername() const {
	return this->_servername;
}

bool Server::setServername(std::string const &attribute, std::fstream &file) {
	(void)file;
	std::stringstream ss(attribute);
	std::string elem;
	ss >> elem;
	if (ss.peek() == EOF) { return false; }
	elem.clear();
	ss >> elem;
	if (ss.peek() != EOF) { return false; }
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
	if (ss.peek() == EOF) { return false; }
	elem.clear();
	ss >> elem;
	if (ss.peek() != EOF) { return false; }
	ss.str("");
	ss.clear();
	ss << elem;
	std::getline(ss, ipAddr, ':');
	if (ss.peek() == EOF) {
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
	if (ss.peek() == EOF) { return false; }
	elem.clear();
	ss >> elem;
	if (ss.peek() != EOF) { return false; }
	this->_root = elem;
	return true;
}

const std::string &Server::getRoot() const {
	return this->_root;
}

bool Server::setLocations(std::string const &attribute, std::fstream &file) {
	std::string line;
	Location location;

	if (location.setLocationPath(attribute) == false) { /* errorhandling; */ }
	while (std::getline(file, line)) {
		// lineCount++;
		if (utils::shouldIgnoreLine(line)) { continue; }
		std::stringstream ss(line);
		std::string elem;
		ss >> elem;
		if (elem.compare("}") == 0) { break; }
		std::map<std::string, bool (Location::*)(std::string const&, std::fstream&)>::iterator iter = this->_setterMap.find(elem);
		if (iter == this->_setterMap.end()) { continue; }
		if (line[line.size() - 1] != ';') { /* errorhandling; */ }
		if (line.size() != 0) { line = line.substr(0, line.size() - 2); }
		if ((location.*(iter->second))(line, file) == false) { /* errorhandling; */ }
	}
	this->_locations.insert(std::pair<std::string, Location>(location.getLocationPath(), location));
	return true;
}

const std::map<std::string, Location> &Server::getLocations() const {
	return this->_locations;
}
