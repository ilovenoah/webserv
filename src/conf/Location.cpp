#include "Location.hpp"
#include <map>

std::map<std::string, void (Location::*)(const std::string &)> Location::_locSetterMap;

Location::Location()
{
	_locSetterMap["location"] = &Location::setLocation;
	_locSetterMap["allow_methods"] = &Location::setAllowMethods;
	_locSetterMap["root"] = &Location::setRoot;
	_locSetterMap["index"] = &Location::setIndex;
	_locSetterMap["cgi_info"] = &Location::setCgiInfo;
}

Location::~Location()
{
}

Location::Location(const Location &copy)
{
	*this = copy;
}

Location &Location::operator=(const Location &copy)
{
	if (this != &copy)
	{
		_location = copy._location;
		_allowMethods = copy._allowMethods;
		_root = copy._root;
		_index = copy._index;
		_cgiInfo = copy._cgiInfo;
	}
	return *this;
}

void Location::setLocation(const std::string &location)
{
	_location = location;
}

void Location::setAllowMethods(const std::string &allowMethods)
{
	_allowMethods = allowMethods;
}

void Location::setRoot(const std::string &root)
{
	_root = root;
}

void Location::setIndex(const std::string &index)
{
	_index = index;
}

void Location::setCgiInfo(const std::string &cgiInfo)
{
	_cgiInfo = cgiInfo;
}

const std::string &Location::getLocation() const
{
	return _location;
}

const std::string &Location::getAllowMethods() const
{
	return _allowMethods;
}

const std::string &Location::getRoot() const
{
	return _root;
}

const std::string &Location::getIndex() const
{
	return _index;
}

const std::string &Location::getCgiInfo() const
{
	return _cgiInfo;
}

void Location::execSetterMap(std::string &keys, std::string &value)
{
	(this->*_locSetterMap[keys])(value);
}
