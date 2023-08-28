#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <string>
#include <map>

class Location
{
  private:
	std::string _location;
	std::string _allowMethods;
	std::string _root;
	std::string _index;
	std::string _cgiInfo;

	std::map<std::string, void (Location::*)(const std::string &)> _locSetterMap;

  public:
	Location();
	~Location();
	Location(const Location &copy);
	Location &operator=(const Location &copy);

	void setLocation(const std::string &location);
	void setAllowMethods(const std::string &allowMethods);
	void setRoot(const std::string &root);
	void setIndex(const std::string &index);
	void setCgiInfo(const std::string &cgiInfo);

	std::string &getLocation() const;
	std::string &getAllowMethods() const;
	std::string &getRoot() const;
	std::string &getIndex() const;
	std::string &getCgiInfo() const;
};

#endif
