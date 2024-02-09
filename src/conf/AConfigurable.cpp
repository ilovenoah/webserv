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
	this->_allowMethods.clear();
	while (ss.peek() != EOF) {
		elem.clear();
		ss >> elem;
		if (elem.empty() == true) { return false; }
		if (elem != "GET" && elem != "POST" && elem != "DELETE") { return false; }
		this->_allowMethods.push_back(elem);
		ss >>  std::ws;
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
	elem.clear();
	ss >> elem;
	ss >> std::ws;
	if (elem.empty() == true) { return false; }
	if (ss.peek() != EOF) { return false; }
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
	while (ss.peek() != EOF) {
		elem.clear();
		ss >> elem;
		if (elem.empty() == true) { return false; }
		this->_index.push_back(elem);
		ss >> std::ws;
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
	elem.clear();
	ss >> elem;
	ss >> std::ws;
	if (ss.peek() != EOF) { return false; }
	if (elem.empty() == true) { return false; }
	if (utils::isNumber(elem) == false) { return false; }
	this->_clientMaxBodySize = utils::decStrToSizeT(elem);
	if (this->_clientMaxBodySize > INT_MAX) { throw std::runtime_error(INVALID_CLIENTMAXBODYSIZE); }
	return true;
}

const ssize_t &AConfigurable::getClientMaxBodySize() const {
	return this->_clientMaxBodySize;
}

bool AConfigurable::setCgiExtensions(std::string const &attribute, std::fstream &file) {
	(void)file;
	std::stringstream ss(attribute);
	std::string elem;
	ss >> elem;
	this->_cgi_extensions.clear();
	while (ss.peek() != EOF) {
		elem.clear();
		ss >> elem;
		if (elem.empty() == true) { return false; }
		if (elem[0] != '.') { return false; };
        for (size_t i = 1; i < elem.length(); ++i) {
            if (!std::isalnum(elem[i])) { return false; }
        }
		this->_cgi_extensions.push_back(elem);
		ss >> std::ws;
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
	elem.clear();
	ss >> elem;
	ss >> std::ws;
	if (elem.empty() == true) { return false; }
	if (ss.peek() != EOF) { return false; }
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
	if (utils::isNumber(str) == false) { return false; }
	return true;
}

bool AConfigurable::setErrorPages(std::string const &attribute, std::fstream &file) {
	(void)file;
	std::stringstream ss(attribute);
	std::string elem;
	std::vector<std::string> statusCodes;
	ss >> elem;
	elem.clear();
	while (true) {
		ss >> elem;
		if (elem.empty() == true) { return false; }
		if (isStatusCode(elem) == false) { break; }
		statusCodes.push_back(elem);
		elem.clear();
	}
	ss >> std::ws;
	if (ss.peek() != EOF) { return false; }
	if (statusCodes.size() == 0) { return false; }
	for (std::vector<std::string>::iterator iter = statusCodes.begin(); iter != statusCodes.end(); ++iter) {
		this->_errorPages.insert(std::pair<std::string, std::string>(*iter, elem));
	}
	return true;
}

const std::map<std::string, std::string> &AConfigurable::getErrorPages() const {
	return this->_errorPages;
}

const std::string &AConfigurable::getReturn() const {
	return this->_return;
}
