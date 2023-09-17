#include "Server.hpp"

#include <vector>

std::map<std::string, void (Server::*)(const std::string &)> Server::_srvSetterMap;

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
		_listen = copy._listen;
		_root = copy._root;
		_allow_methods = copy._allow_methods;
		_index = copy._index;
		_client_body_limit = copy._client_body_limit;
		_location = copy._location;
		_error_page = copy._error_page;
	}
	return *this;
}

void Server::setServerName(const std::string &values) {
	_server_name = values;
}

void Server::setListen(const std::string &values) {
	_listen = values;
}

void Server::setRoot(const std::string &values) {
	_root = values;
}

void Server::setAllowMethods(const std::string &values) {
	_allow_methods = values;
}

void Server::setAutoIndex(const std::string &values) {
	_autoindex = values;
}

void Server::setIndex(const std::string &values) {
	_index = values;
}

void Server::setClientBodyLimit(const std::string &values) {
	_client_body_limit = values;
}

void Server::setCgiInfo(const std::string &values) {
	_cgi_info = values;
}

void Server::setLocation(const std::string &values) {
	std::string key;
	std::string value;
	std::string line;
	std::size_t pos;

	pos = values.find_last_of(' ');
	key = values.substr(0, pos);
	value = values.substr(pos + 1);

	_location.push_back(Location());
	_location.back().setLocation(key);
	while (getline(, line)) {
		_location.back().execSetterMap(key, value);
	}
}

void Server::setErrorPage(const std::string &values) {
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

void Server::setReturn(const std::string &values) {
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

const std::string &Server::getListen() const {
	return _listen;
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
	_fileStream = fileStream;

	try {
		if (_srvSetterMap.find(keys) == _srvSetterMap.end()) {
			throw GenericException(FAIL_KEY + keys);
		} else {
			(this->*_srvSetterMap[keys])(value);
		}
	} catch (GenericException &e) {
		std::cerr << e.what() << std::endl;
	}
}
