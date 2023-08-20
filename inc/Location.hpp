#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <string>

class Location
{
private:
	std::string _location;
	std::string _allowMethods;
	std::string _root;
	std::string _index;
	std::string _cgiInfo;

public:
	Location();
	~Location();
};

#endif
