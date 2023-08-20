#ifndef SEVER_HPP
#define SEVER_HPP

#include "Location.hpp"
#include <map>
#include <string>

class Server
{
private:
	std::string _server_name;
	std::string _listen;
	std::string _root;
	std::string _allow_methods;
	std::string _index;
	std::string _client_body_limit;
	Location _location;
	std::map<std::string, std::string> _error_page;

public:
	Server();
	~Server();
	Server(const Server &copy);
	Server &operator=(const Server &copy);

	void setServerName(const std::string &serverName);
	void setListen(const std::string &listen);
	void setRoot(const std::string &root);
	void setAllowMethods(const std::string &allowMethods);
	void setIndex(const std::string &index);
	void setClientBodyLimit(const std::string &clientBodyLimit);
	void setLocation(const Location &location);
	void setErrorPage(const std::map<std::string, std::string> &errorPage);

	const std::string &getServerName() const;
	const std::string &getListen() const;
	const std::string &getRoot() const;
	const std::string &getAllowMethods() const;
	const std::string &getIndex() const;
	const std::string &getClientBodyLimit() const;
	const Location &getLocation() const;
	const std::map<std::string, std::string> &getErrorPage() const;
};

#endif
