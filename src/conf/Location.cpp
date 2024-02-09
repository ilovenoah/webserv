#include "Location.hpp"

bool Location::setLocationPath(std::string const &attribute) {
	std::stringstream ss(attribute);
	std::string elem;

	ss >> elem;
	elem.clear();
	ss >> elem;
	ss >> std::ws;
	if (elem.empty() == true) { return false; }
	this->_path = elem;
	elem.clear();
	ss >> elem;
	ss >> std::ws;
	if (elem.empty() == true) { return false; }
	if (elem.compare("{") != 0) { return false; }
	if (ss.peek() != EOF) { return false; }
	return true;
}

const std::string &Location::getLocationPath() const {
	return this->_path;
}
