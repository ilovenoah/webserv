#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <string>
#include <fstream>
#include <sstream>

class Location {
	private:
		std::string _path;
		std::vector<std::string> _allowMethods;
		bool _autoindex;
		std::vector<std::string> _index;
		std::size_t _clientMaxBodySize;
		std::vector<std::string> _cgi_extensions;
		std::string _return;
		std::map<std::string, std::string> _errorPages;
		std::size_t _lineCount;

	public:
		bool setLocationPath(std::string const &attribute);
};

#endif
