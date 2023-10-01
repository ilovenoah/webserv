#ifndef LOCATION_HPP
#define LOCATION_HPP

#include "Error.hpp"
#include "env.hpp"

class Location {
   private:
	std::string _location;
	std::string _allowMethods;
	std::string _root;
	std::string _index;
	std::string _cgiInfo;

	static std::map<std::string, void (Location::*)(const std::string &)> _locSetterMap;

   public:
	Location();
	~Location();
	Location(const Location &copy);
	Location &operator=(const Location &copy);

	void setLocation(const std::string &values);
	void setAllowMethods(const std::string &values);
	void setRoot(const std::string &values);
	void setIndex(const std::string &values);
	void setCgiInfo(const std::string &values);

	const std::string &getLocation() const;
	const std::string &getAllowMethods() const;
	const std::string &getRoot() const;
	const std::string &getIndex() const;
	const std::string &getCgiInfo() const;

	void execSetterMap(std::string &keys, std::string &value);
};

#endif
