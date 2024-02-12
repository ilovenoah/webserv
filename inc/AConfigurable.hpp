#ifndef CONFIGURABLE_HPP
#define CONFIGURABLE_HPP

#include <limits.h>

#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "errormsg.hpp"
#include "utils.hpp"

#define ONEMEGA 1000000

class AConfigurable {
	public:
		enum IsAutoIndexed { FALSE = 0, TRUE = 1, UNDEFINED = 2 };

	private:
	protected:
		std::vector<std::string> _allowMethods;
		IsAutoIndexed _autoindex;
		std::vector<std::string> _index;
		ssize_t _clientMaxBodySize;
		std::vector<std::string> _cgi_extensions;
		std::string _return;
		std::map<std::string, std::string> _errorPages;
		std::string _uploadPass;
		std::string _aliasDirective;

	public:
		AConfigurable();
		virtual ~AConfigurable() = 0;
		bool setAllowMethods(std::string const &attribute, std::fstream &file);
		const std::vector<std::string> &getAllowMethods() const;
		bool setAutoIndex(std::string const &attribute, std::fstream &file);
		const IsAutoIndexed &getAutoindex() const;
		bool setIndex(std::string const &attribute, std::fstream &file);
		const std::vector<std::string> &getIndex() const;
		bool setClientMaxBodySize(std::string const &attribute,
								  std::fstream &file);
		const ssize_t &getClientMaxBodySize() const;
		bool setCgiExtensions(std::string const &attribute, std::fstream &file);
		const std::vector<std::string> &getCgiExtensions() const;
		bool setReturn(std::string const &attribute, std::fstream &file);
		bool setErrorPages(std::string const &attribute, std::fstream &file);
		const std::string &getReturn() const;
		const std::map<std::string, std::string> &getErrorPages() const;
		bool setUploadPass(std::string const &attribute, std::fstream &file);
		const std::string &getUploadPass() const;
		bool setAliasDirective(std::string const &attribute, std::fstream &file);
		const std::string &getAliasDirective() const;
};

#endif