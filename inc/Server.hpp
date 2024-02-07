#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <string>
#include <map>
#include "Location.hpp"
#include "utils.hpp"

#define ONEMEGA 1000000
#define MIN_USERPORT 1024
#define MAX_USERPORT 49151

class Server {
	private:
		std::string _servername;
		std::string _ipAddr;
		std::string _port;
		std::vector<std::string> _allowMethods;
		bool _autoindex;
		std::string _index;
		std::size_t _clientMaxBodySize;
		std::vector<std::string> _cgi_extensions;
		std::map<std::string, Location> _locations;
		std::map<std::string, std::string> _returns;
		std::map<std::string, std::string> _errorPages;

	public:
		Server();
		const std::string &getServername() const;
		bool setServername(std::string const &attribute, std::fstream &file);
		bool setListen(std::string const &attribute, std::fstream &file);
		std::string getListen() const;
		const std::string &getIpaddr() const;
		bool setIpaddr(std::string const &ipaddr);
		const std::string &getPort() const;
		bool setPort(std::string const &port);
		const std::vector<std::string> &getAllowMethods() const;
		const bool &getAutoindex() const;
		bool setAutoindex(bool const &autoindex);
		const std::string &getIndex() const;
		bool setIndex(std::string const &index);
		const std::size_t &getClientMaxBodySize() const;
		bool setClientMaxBodySize(std::size_t const &clientMaxBodySize);
		const std::map<std::string, Location> &getLocations() const;
		const std::map<std::string, std::string> &getReturns() const;
		const std::map<std::string, std::string> &getErrorPages() const;
};

#endif