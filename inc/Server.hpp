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
	std::string _index;
	std::string _client_body_limit;
	Location _location;
	std::map<std::string, std::string> _error_page;

  protected:
	std::map<std::string, void (Server::*)(const std::string &)> _srvSetterMap;

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
	void setLocation(const std::string &location);
	void setErrorPage(const std::string &errorPage);

	std::string &getServerName() const;
	std::string &getListen() const;
	std::string &getRoot() const;
	std::string &getAllowMethods() const;
	std::string &getIndex() const;
	std::string &getClientBodyLimit() const;
	Location &getLocation() const;
	std::map<std::string, std::string> &getErrorPage() const;

	void execSetterMap(std::string keys, std::string value);
};

#endif
