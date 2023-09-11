#ifndef SERVER_HPP
#define SERVER_HPP

#include "Location.hpp"
#include <map>

class Server
{
  private:
	std::string _server_name;
	std::string _listen;
	std::string _root;
	std::string _allow_methods;
	std::string _autoindex;
	std::string _index;
	std::string _client_body_limit;
	std::string _return;
	std::string _cgi_info;
	Location _location;
	std::map<std::string, std::string> _error_page;

	static std::map<std::string, void (Server::*)(const std::string &)> _srvSetterMap;

  public:
	Server();
	~Server();
	Server(const Server &copy);
	Server &operator=(const Server &copy);

	void setServerName(const std::string &values);
	void setListen(const std::string &values);
	void setRoot(const std::string &values);
	void setAllowMethods(const std::string &values);
	void setAutoIndex(const std::string &values);
	void setIndex(const std::string &values);
	void setClientBodyLimit(const std::string &values);
	void setReturn(const std::string &values);
	void setCgiInfo(const std::string &values);
	void setLocation(const std::string &values);
	void setErrorPage(const std::string &values);

	const std::string &getServerName() const;
	const std::string &getListen() const;
	const std::string &getRoot() const;
	const std::string &getAllowMethods() const;
	const std::string &getAutoIndex() const;
	const std::string &getIndex() const;
	const std::string &getClientBodyLimit() const;
	const std::string &getReturn() const;
	const std::string &getCgiInfo() const;
	const Location &getLocation() const;
	const std::map<std::string, std::string> &getErrorPage() const;

	void execSetterMap(std::string &keys, std::string &value);
};

#endif
