#include "Location.hpp"

Location::Location() : AConfigurable() {
	this->_allowMethods.clear();
	this->_autoindex = AConfigurable::UNDEFINED;
	this->_index.clear();
	this->_clientMaxBodySize = -1;
	this->_cgi_extensions.clear();
	this->_return.clear();
	this->_errorPages.clear();
}

bool Location::setLocationPath(std::string const &attribute) {
	std::stringstream ss(attribute);
	std::string elem;

	ss >> elem;
	elem.clear();
	ss >> elem;
	ss >> std::ws;
	if (elem.empty() == true) {
		return false;
	}
	if (elem[elem.length() - 1] == '/' && elem.length() > 1) {
		elem.erase(elem.length() - 1);
	}
	this->_path = elem;
	elem.clear();
	ss >> elem;
	ss >> std::ws;
	if (elem.empty() == true) {
		return false;
	}
	if (elem.compare("{") != 0) {
		return false;
	}
	if (ss.peek() != EOF) {
		return false;
	}
	return true;
}

const std::string &Location::getLocationPath() const { return this->_path; }

void Location::fillLocationDirectives(Server const &server) {
	if (this->_allowMethods.size() == 0) {
		this->_allowMethods = server.getAllowMethods();
	}
	if (this->_autoindex == AConfigurable::UNDEFINED) {
		this->_autoindex = server.getAutoindex();
	}
	if (this->_index.size() == 0) {
		this->_index = server.getIndex();
	}
	if (this->_clientMaxBodySize == -1) {
		this->_clientMaxBodySize = server.getClientMaxBodySize();
	}
	if (this->_cgi_extensions.size() == 0) {
		this->_cgi_extensions = server.getCgiExtensions();
	}
	if (this->_return.empty() == true) {
		this->_return = server.getReturn();
	}
	if (this->_errorPages.size() == 0) {
		this->_errorPages = server.getErrorPages();
	}
	if (this->_uploadPass.empty() == true) {
		this->_uploadPass = server.getUploadPass();
	}
	if (this->_aliasDirective.empty() == true) {
		this->_aliasDirective = server.getAliasDirective();
	}
	if (this->_root.empty() == true) {
		this->_root = server.getRoot();
	}
}