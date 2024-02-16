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
		std::string _root;
		std::vector<std::string> _allowMethods;
		IsAutoIndexed _autoindex;
		std::vector<std::string> _index;
		ssize_t _clientMaxBodySize;
		std::vector<std::string> _cgi_extensions;
		std::string _return;
		std::map<std::string, std::string> _errorPages;
		std::string _uploadStore;

	public:
		AConfigurable();
		virtual ~AConfigurable() = 0;
		virtual bool setRoot(std::string const &attribute, std::fstream &file);
		const std::string &getRoot() const;
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
		bool setuploadStore(std::string const &attribute, std::fstream &file);
		const std::string &getuploadStore() const;
};

#endif