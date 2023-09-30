#ifndef SERVER_HPP
#define SERVER_HPP

#include "Location.hpp"
#include "env.hpp"

class Server {
   private:
	std::string _server_name;
	std::string _ip_address;
	std::string _port;
	std::string _root;
	std::string _allow_methods;
	std::string _autoindex;
	std::string _index;
	std::string _client_body_limit;
	std::string _cgi_info;
	std::vector<Location> _location;
	std::map<std::string, std::string> _error_page;
	std::map<std::string, std::string> _return;

	static std::map<std::string, void (Server::*)(const std::string &, std::ifstream &)> _srvSetterMap;

   public:
	Server();
	~Server();
	Server(const Server &copy);
	Server &operator=(const Server &copy);

	void setServerName(const std::string &values, std::ifstream &fileStream);
	void setListen(const std::string &values, std::ifstream &fileStream);
	void setRoot(const std::string &values, std::ifstream &fileStream);
	void setAllowMethods(const std::string &values, std::ifstream &fileStream);
	void setAutoIndex(const std::string &values, std::ifstream &fileStream);
	void setIndex(const std::string &values, std::ifstream &fileStream);
	void setClientBodyLimit(const std::string &values, std::ifstream &fileStream);
	void setCgiInfo(const std::string &values, std::ifstream &fileStream);
	void setLocation(const std::string &values, std::ifstream &fileStream);
	void setErrorPage(const std::string &values, std::ifstream &fileStream);
	void setReturn(const std::string &values, std::ifstream &fileStream);

	const std::string &getServerName() const;
	const std::string &getIpAddress() const;
	const std::string &getPort() const;
	const std::string getListen();
	const std::string &getRoot() const;
	const std::string &getAllowMethods() const;
	const std::string &getAutoIndex() const;
	const std::string &getIndex() const;
	const std::string &getClientBodyLimit() const;
	const std::string &getCgiInfo() const;
	const std::vector<Location> &getLocation() const;
	const std::map<std::string, std::string> &getErrorPage() const;
	const std::map<std::string, std::string> &getReturn() const;

	void execSetterMap(std::string &keys, std::string &value, std::ifstream &fileStream);
};

#endif
