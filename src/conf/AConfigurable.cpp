#include "AConfigurable.hpp"

static std::vector<std::string> initAllowedMethods() {
	std::vector<std::string> methods;
	methods.push_back("GET");
	methods.push_back("POST");
	methods.push_back("DELETE");
	return methods;
}

AConfigurable::AConfigurable()
	: _allowMethods(initAllowedMethods()),
	  _autoindex(AConfigurable::FALSE),
	  _clientMaxBodySize(ONEMEGA) {}

AConfigurable::~AConfigurable() {}

bool AConfigurable::setRoot(std::string const &attribute, std::fstream &file) {
	(void)file;
	std::stringstream ss(attribute);
	std::string elem;
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
		this->_root = elem;
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
	this->_root = elem;
	return true;
}

const std::string &AConfigurable::getRoot() const { return this->_root; }

bool AConfigurable::isAllowedMethod(const std::string &method) const {
	for (std::vector<std::string>::const_iterator iter =
			 this->_allowMethods.begin();
		 iter != this->_allowMethods.end(); ++iter) {
		if (iter->compare(method) == 0) {
			return true;
		}
	}
	return false;
}

bool AConfigurable::setAllowMethods(std::string const &attribute,
									std::fstream &file) {
	(void)file;
	std::stringstream ss(attribute);
	std::string elem;
	ss >> elem;
	this->_allowMethods.clear();
	while (ss.peek() != EOF) {
		elem.clear();
		ss >> elem;
		if (elem.empty() == true) {
			return false;
		}
		if (elem != "GET" && elem != "POST" && elem != "DELETE") {
			return false;
		}
		this->_allowMethods.push_back(elem);
		ss >> std::ws;
	}
	return true;
}

const std::vector<std::string> &AConfigurable::getAllowMethods() const {
	return this->_allowMethods;
}
bool AConfigurable::setAutoIndex(std::string const &attribute,
								 std::fstream &file) {
	(void)file;
	std::stringstream ss(attribute);
	std::string elem;
	ss >> elem;
	elem.clear();
	ss >> elem;
	ss >> std::ws;
	if (elem.empty() == true) {
		return false;
	}
	if (ss.peek() != EOF) {
		return false;
	}
	if (elem == "on") {
		this->_autoindex = AConfigurable::TRUE;
	} else if (elem == "off") {
		this->_autoindex = AConfigurable::FALSE;
	} else {
		return false;
	}
	return true;
}

const AConfigurable::IsAutoIndexed &AConfigurable::getAutoindex() const {
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
		if (elem.empty() == true) {
			return false;
		}
		this->_index.push_back(elem);
		ss >> std::ws;
	}
	return true;
}

const std::vector<std::string> &AConfigurable::getIndex() const {
	return this->_index;
}

static ssize_t byteConverter(std::string const &elem) {
	if (elem.empty() == true) {
		return 1;
	} else if (elem == "k") {
		return 1000;
	} else if (elem == "m") {
		return 1000000;
	} else {
		return 1000000000;
	}
}

bool AConfigurable::setClientMaxBodySize(std::string const &attribute,
										 std::fstream &file) {
	(void)file;
	std::stringstream ss(attribute);
	std::string elem;
	ssize_t byte;
	ss >> elem;
	elem.clear();
	ss >> byte;
	if (ss.fail()) {
		return false;
	}
	ss >> elem;
	ss >> std::ws;
	if (ss.peek() != EOF) {
		return false;
	}
	if (elem.empty() == false && (elem != "k" && elem != "m" && elem != "g")) {
		return false;
	}
	this->_clientMaxBodySize = byte * byteConverter(elem);
	if (this->_clientMaxBodySize > INT_MAX || this->_clientMaxBodySize < 0) {
		throw std::runtime_error(INVALID_CLIENTMAXBODYSIZE);
	}
	return true;
}

const ssize_t &AConfigurable::getClientMaxBodySize() const {
	return this->_clientMaxBodySize;
}

static std::string getRuntimePath(std::string const &runtimeName) {
	std::string runTimePath;
	const char *pathEnv(std::getenv("PATH"));
	if (pathEnv == NULL) {
		return "";
	}
	std::string pathEnvStr(pathEnv);
	std::stringstream ss(pathEnvStr);
	std::string elem;
	while (std::getline(ss, elem, ':')) {
		if (elem.length() != 0 && elem.find_last_of('/') - 1 != elem.length()) {
			elem.append("/");
		}
		if (utils::isAccess(elem + runtimeName, X_OK) == true) {
			return elem + runtimeName;
		}
	}
	return "";
}

static std::string getRuntimeName(const std::string &extension) {
	if (extension.compare(".py") == 0) {
		return "python3";
	} else if (extension.compare(".php") == 0) {
		return "php";
	} else if (extension.compare(".pl") == 0) {
		return "perl";
	}
	return "";
}

bool AConfigurable::setCgiExtensions(std::string const &attribute,
									 std::fstream &file) {
	(void)file;
	std::stringstream ss(attribute);
	std::string elem;
	ss >> elem;
	this->_cgi_extensions.clear();
	while (ss.peek() != EOF) {
		elem.clear();
		ss >> elem;
		if (elem.empty() == true) {
			return false;
		}
		if (elem[0] != '.') {
			return false;
		};
		for (size_t i = 1; i < elem.length(); ++i) {
			if (!std::isalnum(elem[i])) {
				return false;
			}
		}
		std::string runtimeName = getRuntimeName(elem);
		if (runtimeName.empty() == true) {
			return false;
		}
		std::string pathRuntimePath = getRuntimePath(runtimeName);
		if (pathRuntimePath.empty() == true) {
			return false;
		}
		if (this->_cgi_extensions.count(elem) > 0) {
			return false;
		}
		this->_cgi_extensions.insert(
			std::pair<std::string, std::string>(elem, pathRuntimePath));
		ss >> std::ws;
	}
	return true;
}

const std::map<std::string, std::string> &AConfigurable::getCgiExtensions()
	const {
	return this->_cgi_extensions;
}

bool AConfigurable::setReturn(std::string const &attribute,
							  std::fstream &file) {
	(void)file;
	std::stringstream ss(attribute);
	std::string elem;
	ss >> elem;
	elem.clear();
	ss >> elem;
	ss >> std::ws;
	if (elem.empty() == true) {
		return false;
	}
	if (ss.peek() != EOF) {
		return false;
	}
	ss.str("");
	ss.clear();
	ss << elem;
	std::string protocol;
	std::getline(ss, protocol, ':');
	if (protocol.compare("http") != 0 && protocol.compare("https") != 0) {
		return false;
	}
	this->_return = elem;
	return true;
}

static bool isStatusCode(std::string const &str) {
	if (str.size() != 3) {
		return false;
	}
	if (utils::isNumber(str) == false) {
		return false;
	}
	return true;
}

bool AConfigurable::setErrorPages(std::string const &attribute,
								  std::fstream &file) {
	(void)file;
	std::stringstream ss(attribute);
	std::string elem;
	std::vector<std::string> statusCodes;
	ss >> elem;
	elem.clear();
	while (true) {
		ss >> elem;
		if (elem.empty() == true) {
			return false;
		}
		if (isStatusCode(elem) == false) {
			break;
		}
		statusCodes.push_back(elem);
		elem.clear();
	}
	ss >> std::ws;
	if (ss.peek() != EOF) {
		return false;
	}
	if (statusCodes.size() == 0) {
		return false;
	}
	for (std::vector<std::string>::iterator iter = statusCodes.begin();
		 iter != statusCodes.end(); ++iter) {
		this->_errorPages.insert(
			std::pair<std::string, std::string>(*iter, elem));
	}
	return true;
}

const std::map<std::string, std::string> &AConfigurable::getErrorPages() const {
	return this->_errorPages;
}

const std::string &AConfigurable::getReturn() const { return this->_return; }

bool AConfigurable::setuploadStore(std::string const &attribute,
								   std::fstream &file) {
	(void)file;
	std::stringstream ss(attribute);
	std::string elem;
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
		this->_root = elem;
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
	this->_uploadStore = elem;
	return true;
}

const std::string &AConfigurable::getuploadStore() const {
	return this->_uploadStore;
}
