#include "Server.hpp"
#include <vector>

Server::Server()
{
	_srvSetterMap["server_name"] = &Server::setServerName;
	_srvSetterMap["listen"] = &Server::setListen;
	_srvSetterMap["root"] = &Server::setRoot;
	_srvSetterMap["allow_methods"] = &Server::setAllowMethods;
	_srvSetterMap["index"] = &Server::setIndex;
	_srvSetterMap["client_body_limit"] = &Server::setClientBodyLimit;
	_srvSetterMap["location"] = &Server::setLocation;
	_srvSetterMap["error_page"] = &Server::setErrorPage;
}

Server::~Server() {}

Server::Server(const Server &copy)
{
	*this = copy;
}

Server &Server::operator=(const Server &copy)
{
	if (this != &copy)
	{
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

void Server::setServerName(const std::string &serverName)
{
	_server_name = serverName;
}

void Server::setListen(const std::string &listen)
{
	_listen = listen;
}

void Server::setRoot(const std::string &root)
{
	_root = root;
}

void Server::setAllowMethods(const std::string &allowMethods)
{
	_allow_methods = allowMethods;
}

void Server::setIndex(const std::string &index)
{
	_index = index;
}

void Server::setClientBodyLimit(const std::string &clientBodyLimit)
{
	_client_body_limit = clientBodyLimit;
}

void Server::setLocation(const std::string &location)
{
	Location loc;
	std::string key;
	std::string value;
	std::size_t pos;

	pos = location.find_last_of(' ');
	key = location.substr(0, pos);
	value = location.substr(pos + 1);

	

}

void Server::setErrorPage(const std::string &errorPage)
{
	std::string keys;
	std::string value;
	std::size_t pos;

	pos = errorPage.find_last_of(' ');
	keys = errorPage.substr(0, pos);
	value = errorPage.substr(pos + 1);

	while ((pos = keys.find(' ')) != std::string::npos)
	{
		_error_page[keys.substr(0, pos)] = value;
		keys.erase(0, pos + 1);
	}
	_error_page[keys] = value;
}

const std::string &Server::getServerName() const
{
	return _server_name;
}

const std::string &Server::getListen() const
{
	return _listen;
}

const std::string &Server::getRoot() const
{
	return _root;
}

const std::string &Server::getAllowMethods() const
{
	return _allow_methods;
}

const std::string &Server::getIndex() const
{
	return _index;
}

const std::string &Server::getClientBodyLimit() const
{
	return _client_body_limit;
}

const Location &Server::getLocation() const
{
	return _location;
}

const std::map<std::string, std::string> &Server::getErrorPage() const
{
	return _error_page;
}
