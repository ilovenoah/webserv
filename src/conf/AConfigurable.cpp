#include "AConfigurable.hpp"

static std::vector<std::string> initAllowedMethods() {
    std::vector<std::string> methods;
    methods.push_back("GET");
    methods.push_back("POST");
    methods.push_back("DELETE");
    return methods;
}

AConfigurable::AConfigurable() : _allowMethods(initAllowedMethods()), _autoindex(false), _clientMaxBodySize(ONEMEGA) {}

AConfigurable::~AConfigurable() {}

bool AConfigurable::setAllowMethods(std::string const &attribute, std::fstream &file) {
	(void)file;
	std::stringstream ss(attribute);
	std::string elem;
	ss >> elem;
	if (ss.eof() == true) { return false; }
	this->_allowMethods.clear();
	while (ss.eof() == false) {
		elem.clear();
		ss >> elem;
		if (elem != "GET" && elem != "POST" && elem != "DELETE") { return false; }
		this->_allowMethods.push_back(elem);
	}
	return true;
}

const std::vector<std::string> &AConfigurable::getAllowMethods() const {
	return this->_allowMethods;
}
bool AConfigurable::setAutoIndex(std::string const &attribute, std::fstream &file) {
	(void)file;
	std::stringstream ss(attribute);
	std::string elem;
	ss >> elem;
	if (ss.eof() == true) { return false; }
	elem.clear();
	ss >> elem;
	if (ss.eof() == false) { return false; }
	if (elem == "on") {
		this->_autoindex = true;
	} else if (elem == "off") {
		this->_autoindex = false;
	} else {
		return false;
	}
	return true;
}

const bool &AConfigurable::getAutoindex() const {
	return this->_autoindex;
}

bool AConfigurable::setIndex(std::string const &attribute, std::fstream &file) {
	(void)file;
	std::stringstream ss(attribute);
	std::string elem;
	ss >> elem;
	if (ss.eof() == true) { return false; }
	while (ss.eof() == false) {
		elem.clear();
		ss >> elem;
		this->_index.push_back(elem);
	}
	return true;
}

const std::vector<std::string> &AConfigurable::getIndex() const {
	return this->_index;
}

bool AConfigurable::setClientMaxBodySize(std::string const &attribute, std::fstream &file) {
	(void)file;
	std::stringstream ss(attribute);
	std::string elem;
	ss >> elem;
	if (ss.eof() == true) { return false; }
	elem.clear();
	ss >> elem;
	if (ss.eof() == false) { return false; }
	this->_clientMaxBodySize = utils::decStrToSizeT(elem);
	return true;
}

const std::size_t &AConfigurable::getClientMaxBodySize() const {
	return this->_clientMaxBodySize;
}

bool AConfigurable::setCgiExtensions(std::string const &attribute, std::fstream &file) {
	(void)file;
	std::stringstream ss(attribute);
	std::string elem;
	ss >> elem;
	if (ss.eof() == true) { return false; }
	this->_cgi_extensions.clear();
	while (ss.eof() == false) {
		elem.clear();
		ss >> elem;
		if (elem[0] != '.') { return false; };
        for (size_t i = 1; i < elem.length(); ++i) {
            if (!std::isalnum(elem[i])) { return false; }
        }
		this->_cgi_extensions.push_back(elem);
	}
	return true;
}

const std::vector<std::string> &AConfigurable::getCgiExtensions() const {
	return this->_cgi_extensions;
}

bool AConfigurable::setReturn(std::string const &attribute, std::fstream &file) {
	(void)file;
	std::stringstream ss(attribute);
	std::string elem;
	ss >> elem;
	if (ss.eof() == true) { return false; }
	elem.clear();
	ss >> elem;
	if (ss.eof() == false) { return false; }
	ss.str("");
	ss.clear();
	ss << elem;
	std::string protocol;
	std::getline(ss, protocol, ':');
	if (protocol.compare("http") != 0 && protocol.compare("https") != 0) { return false ;}
	this->_return = elem;
	return true;
}

static bool isStatusCode(std::string const &str) {
	if (str.size() != 3) { return false; }
	for (std::string::const_iterator iter = str.begin(); iter != str.end(); ++iter) {
		if (std::isdigit(*iter) == false) { return false; }
	}
	return true;
}

bool AConfigurable::setErrorPages(std::string const &attribute, std::fstream &file) {
	(void)file;
	std::stringstream ss(attribute);
	std::string elem;
	std::vector<std::string> statusCodes;
	ss >> elem;
	if (ss.eof() == true) { return false; }
	elem.clear();
	while (ss.eof() == false) {
		ss >> elem;
		if (isStatusCode(elem) == false) { break; }
		statusCodes.push_back(elem);
	}
	if (ss.eof() == false) { return false; }
	if (statusCodes.size() == 0) { return false; }
	for (std::vector<std::string>::iterator iter = statusCodes.begin(); iter != statusCodes.end(); ++iter) {
		this->_errorPages.insert(std::pair<std::string, std::string>(*iter, elem));
	}
	return true;
}

const std::string &AConfigurable::getReturn() const {
	return this->_return;
}
