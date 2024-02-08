#include "Location.hpp"

bool Location::setLocationPath(std::string const &attribute) {
	std::stringstream ss(attribute);
	std::string elem;

	ss >> elem;
	if (ss.eof() == true) { return false; }
	elem.clear();
	ss >> elem;
	if (ss.eof() == true) { return false; }
	this->_path = elem;
	elem.clear();
	ss >> elem;
	if (ss.eof() == false) { return false; }
	if (elem.compare("{") != 0) { return false; }
	return true;
}