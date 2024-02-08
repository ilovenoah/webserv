#ifndef CONFIGURABLE_HPP
#define CONFIGURABLE_HPP

#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <sstream>
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
		virtual bool setAllowMethods(std::string const &attribute, std::fstream &file) = 0;
		virtual const std::vector<std::string> &getAllowMethods() const = 0;
		virtual bool setAutoIndex(std::string const &attribute, std::fstream &file) = 0;
		virtual const bool &getAutoindex() const = 0;
		virtual bool setIndex(std::string const &attribute, std::fstream &file) = 0;
		virtual const std::vector<std::string> &getIndex() const = 0;
		virtual bool setClientMaxBodySize(std::string const &attribute, std::fstream &file) = 0;
		virtual const std::size_t &getClientMaxBodySize() const = 0;
		virtual bool setCgiExtensions(std::string const &attribute, std::fstream &file) = 0;
		virtual const std::vector<std::string> &getCgiExtensions() const = 0;
		virtual bool setReturn(std::string const &attribute, std::fstream &file) = 0;
		virtual bool setErrorPages(std::string const &attribute, std::fstream &file) = 0;
		virtual const std::string &getReturn() const = 0;
		virtual const std::map<std::string, std::string> &getErrorPages() const = 0;
};

#endif