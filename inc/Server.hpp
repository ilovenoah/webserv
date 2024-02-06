#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <string>
#include <map>

class Server {
	private:
		std::string _servername;
		std::string _ipAddr;
		std::string _port;
		std::string _allowMethods;
		bool _autoindex;
		std::string _index;
		std::size_t _clientMaxBodySize;
		std::vector<std::string> _cgi_extensions;
		std::map<std::string, Location> _locations;
		std::map<std::string, std::string> _returns;
		std::map<std::string, std::string> _errorPages;

	public:
		const std::string &getServername() const;
		void setServername(std::string const &servername);
		const std::string &getIpaddr() const;
		void setIpaddr(std::string const &ipaddr);
		const std::string &getPort() const;
		void setPort(std::string const &port);
		const std::string &getAllowMethods() const;
		void setAllowMethods(std::string const &allowMethods);
		const bool &getAutoindex() const;
		void setAutoindex(bool const &autoindex);
		const std::string &getIndex() const;
		void setIndex(std::string const &index);
		const std::size_t &getClientMaxBodySize() const;
		void setClientMaxBodySize(std::size_t const &clientMaxBodySize);
		const std::map<std::string, Location> &getLocations() const;
		const std::map<std::string, std::string> &getReturns() const;
		const std::map<std::string, std::string> &getErrorPages() const;
};

#endif