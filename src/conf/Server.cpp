#include "Server.hpp"

#include <vector>

#include "Config.hpp"

std::map<std::string, void (Server::*)(const std::string &, std::ifstream &)> Server::_srvSetterMap;

Server::Server() {
	_srvSetterMap["server_name"] = &Server::setServerName;
	_srvSetterMap["listen"] = &Server::setListen;
	_srvSetterMap["root"] = &Server::setRoot;
	_srvSetterMap["allow_methods"] = &Server::setAllowMethods;
	_srvSetterMap["autoindex"] = &Server::setAutoIndex;
	_srvSetterMap["index"] = &Server::setIndex;
	_srvSetterMap["client_body_limit"] = &Server::setClientBodyLimit;
	_srvSetterMap["cgi_info"] = &Server::setCgiInfo;
	_srvSetterMap["return"] = &Server::setReturn;
	_srvSetterMap["location"] = &Server::setLocation;
	_srvSetterMap["error_page"] = &Server::setErrorPage;
}

Server::~Server() {
}

Server::Server(const Server &copy) {
	*this = copy;
}

Server &Server::operator=(const Server &copy) {
	if (this != &copy) {
		_server_name = copy._server_name;
		_ip_address = copy._ip_address;
		_port = copy._port;
		_root = copy._root;
		_allow_methods = copy._allow_methods;
		_autoindex = copy._autoindex;
		_index = copy._index;
		_client_body_limit = copy._client_body_limit;
		_cgi_info = copy._cgi_info;
		_location = copy._location;
		_error_page = copy._error_page;
		_return = copy._return;
	}
	return *this;
}

void Server::setServerName(const std::string &values, std::ifstream &fileStream) {
	(void)fileStream;
	_server_name = values;
}

void Server::setListen(const std::string &values, std::ifstream &fileStream) {
	(void)fileStream;
	std::size_t pos;

	_port = values;
	pos = values.find(':');
	if (pos != std::string::npos) {
		_ip_address = values.substr(0, pos);
		_port = values.substr(pos + 1);
	}
}

void Server::setRoot(const std::string &values, std::ifstream &fileStream) {
	(void)fileStream;
	_root = values;
}

void Server::setAllowMethods(const std::string &values, std::ifstream &fileStream) {
	(void)fileStream;

	if (values.find("GET") == std::string::npos && values.find("POST") == std::string::npos && values.find("DELETE") == std::string::npos) {
		throw GenericException(CONFIG_ERROR);
	}
	_allow_methods = values;
}

void Server::setAutoIndex(const std::string &values, std::ifstream &fileStream) {
	(void)fileStream;
	if (values != "on" && values != "off") {
		throw GenericException(CONFIG_ERROR);
	}
	_autoindex = values;
}

void Server::setIndex(const std::string &values, std::ifstream &fileStream) {
	(void)fileStream;
	_index = values;
}

void Server::setClientBodyLimit(const std::string &values, std::ifstream &fileStream) {
	(void)fileStream;

	std::istringstream iss(values);
	std::size_t num;
	if (!(iss >> num) || iss.peek() != EOF || num < 0 || num > INT_MAX) {
		throw std::runtime_error(CONFIG_ERROR);
	}

	_client_body_limit = num;
}

void Server::setCgiInfo(const std::string &values, std::ifstream &fileStream) {
	(void)fileStream;
	_cgi_info = values;
}

void Server::setLocation(const std::string &values, std::ifstream &fileStream) {
	std::string key;
	std::string value;
	std::string line;
	std::size_t pos;
	Config config;

	pos = values.find_last_of(' ');
	key = values.substr(0, pos);
	value = values.substr(pos + 1);

	_location.push_back(Location());
	_location.back().setLocation(key);
	while (std::getline(fileStream, line)) {
		if (line.find('}') != std::string::npos) {
			break;
		}
		config.removeUnwanted(line);
		config.removeComments(line);
		if (line.empty()) {
			continue;
		}
		key = line.substr(0, line.find(' '));
		value = line.substr(line.find(' ') + 1);
		_location.back().execSetterMap(key, value);
	}
}

void Server::setErrorPage(const std::string &values, std::ifstream &fileStream) {
	(void)fileStream;
	std::string keys;
	std::string value;
	std::size_t pos;

	pos = values.find_last_of(' ');
	keys = values.substr(0, pos);
	value = values.substr(pos + 1);

	while ((pos = keys.find(' ')) != std::string::npos) {
		_error_page[keys.substr(0, pos)] = value;
		keys.erase(0, pos + 1);
	}
	_error_page[keys] = value;
}

void Server::setReturn(const std::string &values, std::ifstream &fileStream) {
	(void)fileStream;
	std::string key;
	std::string value;
	std::size_t pos;

	pos = values.find_last_of(' ');
	key = values.substr(0, pos);
	value = values.substr(pos + 1);

	while ((pos = key.find(' ')) != std::string::npos) {
		_return[key.substr(0, pos)] = value;
		key.erase(0, pos + 1);
	}
	_return[key] = value;
}

const std::string &Server::getServerName() const {
	return _server_name;
}

const std::string &Server::getIpAddress() const {
	return _ip_address;
}

const std::string &Server::getPort() const {
	return _port;
}

std::string Server::getListen() const {
	std::string ipAddress = getIpAddress();
	std::string port = getPort();

	return ipAddress + ":" + port;
}

const std::string &Server::getRoot() const {
	return _root;
}

const std::string &Server::getAllowMethods() const {
	return _allow_methods;
}

const std::string &Server::getAutoIndex() const {
	return _autoindex;
}

const std::string &Server::getIndex() const {
	return _index;
}

const std::string &Server::getClientBodyLimit() const {
	return _client_body_limit;
}

const std::string &Server::getCgiInfo() const {
	return _cgi_info;
}

const std::vector<Location> &Server::getLocation() const {
	return _location;
}

const std::map<std::string, std::string> &Server::getErrorPage() const {
	return _error_page;
}

const std::map<std::string, std::string> &Server::getReturn() const {
	return _return;
}

void Server::execSetterMap(std::string &keys, std::string &value, std::ifstream &fileStream) {
	try {
		if (_srvSetterMap.find(keys) == _srvSetterMap.end()) {
			throw GenericException(UNKOWN_KEY + keys);
		} else {
			(this->*_srvSetterMap[keys])(value, fileStream);
		}
	} catch (GenericException &e) {
		std::cerr << e.what() << std::endl;
		exit(1);
	}
}
