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
		std::string _root;
		std::vector<std::string> _allowMethods;
		bool _autoindex;
		std::vector<std::string> _index;
		std::size_t _clientMaxBodySize;
		std::vector<std::string> _cgi_extensions;
		std::string _return;
		std::map<std::string, Location> _locations;
		std::map<std::string, std::string> _errorPages;

	public:
		Server();
		bool setServername(std::string const &attribute, std::fstream &file);
		const std::string &getServername() const;
		bool setListen(std::string const &attribute, std::fstream &file);
		std::string getListen() const;
		const std::string &getIpaddr() const;
		const std::string &getPort() const;
		bool setRoot(std::string const &attribute, std::fstream &file);
		const std::string &getRoot() const;
		bool setAllowMethods(std::string const &attribute, std::fstream &file);
		const std::vector<std::string> &getAllowMethods() const;
		bool setAutoIndex(std::string const &attribute, std::fstream &file);
		const bool &getAutoindex() const;
		bool setIndex(std::string const &attribute, std::fstream &file);
		const std::vector<std::string> &getIndex() const;
		bool setClientMaxBodySize(std::string const &attribute, std::fstream &file);
		const std::size_t &getClientMaxBodySize() const;
		bool setCgiExtensions(std::string const &attribute, std::fstream &file);
		const std::vector<std::string> &getCgiExtensions() const;
		bool setReturn(std::string const &attribute, std::fstream &file);
		const std::string &getReturn() const;
		const std::map<std::string, Location> &getLocations() const;
		const std::map<std::string, std::string> &getErrorPages() const;
};

#endif