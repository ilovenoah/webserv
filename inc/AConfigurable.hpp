#ifndef CONFIGURABLE_HPP
#define CONFIGURABLE_HPP

#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <limits.h>
#include "utils.hpp"

#define ONEMEGA 1000000

class AConfigurable {
	private:
		std::vector<std::string> _allowMethods;
		bool _autoindex;
		std::vector<std::string> _index;
		std::size_t _clientMaxBodySize;
		std::vector<std::string> _cgi_extensions;
		std::string _return;
		std::map<std::string, std::string> _errorPages;

	public:
		AConfigurable();
		virtual ~AConfigurable() = 0;
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
		bool setErrorPages(std::string const &attribute, std::fstream &file);
		const std::string &getReturn() const;
		const std::map<std::string, std::string> &getErrorPages() const;
};

#endif