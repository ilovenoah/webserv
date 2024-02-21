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

bool Location::setRoot(std::string const &attribute, std::fstream &file) {
	(void)file;
	std::stringstream ss(attribute);
	std::string elem;
	if (this->_aliasDirective.empty() == false) {
		return false;
	}
	ss >> elem;
	if (ss.peek() == EOF) {
		return false;
	}
	elem.clear();
	ss >> elem;
	if (ss.peek() != EOF) {
		return false;
	}
	// isAccess
	Result<bool, std::string> res = utils::isDirectory(elem);
	if (res.isError() == true) {
		return false;
	}
	if (res.getOk() == false) {
		return false;
	}
	if (elem[0] != '/' && elem.find("./") != 0) {
		elem = "./" + elem;
	}
	if ((elem.compare("/") != 0 && elem.compare("./") != 0) &&
		elem.find_last_of('/') == elem.length() - 1) {
		elem.erase(elem.length() - 1);
	}
	this->_root = elem;
	return true;
}

bool Location::setAliasDirective(std::string const &attribute,
								 std::fstream &file) {
	(void)file;
	std::stringstream ss(attribute);
	std::string elem;
	if (this->_root.empty() == false) {
		return false;
	}
	ss >> elem;
	if (ss.peek() == EOF) {
		return false;
	}
	elem.clear();
	ss >> elem;
	if (ss.peek() != EOF) {
		return false;
	}
	// isAccess
	Result<bool, std::string> res = utils::isDirectory(elem);
	if (res.isError() == true) {
		return false;
	}
	if (res.getOk() == false) {
		return false;
	}
	if (elem.find("..") != std::string::npos) {
		return false;
	}
	if (elem == "/") {
		this->_aliasDirective = elem;
		return true;
	}
	if (elem.find("./") == 0 || elem == ".") {
		const char *env_p = std::getenv("PWD");
		if (env_p == NULL) {
			throw std::runtime_error(PWD_NOT_FOUND);
		}
		if (elem == ".") {
			elem = env_p;
		} else {
			elem = env_p + elem.substr(1);
		}
	}
	elem = utils::replaceUri(elem, ".", "");
	elem = utils::replaceUri(elem, "//", "/");
	if (elem.find_last_of('/') == elem.length() - 1) {
		elem.erase(elem.length() - 1);
	}
	this->_aliasDirective = elem;
	return true;
}

const std::string &Location::getAliasDirective() const {
	return this->_aliasDirective;
}

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
	if (this->_uploadStore.empty() == true) {
		this->_uploadStore = server.getuploadStore();
	}
	if (this->_root.empty() == true && this->_aliasDirective.empty() == true) {
		this->_root = server.getRoot();
	}
	for (std::map<std::string, std::string>::const_iterator iter =
			 server.getErrorPages().begin();
		 iter != server.getErrorPages().end(); ++iter) {
		this->_errorPages.insert(
			std::pair<std::string, std::string>(iter->first, iter->second));
	}
}
