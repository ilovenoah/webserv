CGIHandler::CGIHandler()
	: _server(NULL), _request(NULL), _isActive(false), _revents(0), _wpfd(0), _rpfd(0) {}


static char *strDupToCharPtr(std::string const &src) {
	std::size_t i_str(0);
	char *str = new(std::nothrow) char[src.length() + 1]();
	if (str == NULL) { return NULL; }
	std::memset(str, 0, src.length() + 1);
	for (std::string::const_iterator iter = src.begin(); iter != src.end(); ++iter) {
		str[i_str] = *iter;
		i_str++;
	}
	return str;
}
	std::string authType;
	Result res = request.getHeaderValue("Authorization");
	if (res.isOK() == true) {
		authType = res.getOk();
	}
	std::string *elem = new(std::nothrow) std::string("AUTH_TYPE=" + authType);
	if (elem == NULL) { return false; }
	this->_env.push_back(elem->c_str());
	return true;
}

bool CGIHandler::setContentLength(const Request &request, const std::string &actPath) {
	(void)actPath;
	std::string contenLength;
	Result res = request.getHeaderValue("Content-Length");
	if (res.isOK() == true) {
		contenLength = res.getOk();
	}
	std::string *elem = new(std::nothrow) std::string("CONTENT_LENGTH=" + contenLength);
	if (elem == NULL) { return false; }
	this->_env.push_back(elem->c_str());
	return true;
}

bool CGIHandler::setContentType(const Request &request, const std::string &actPath) {
	(void)actPath;
	std::string contentType;
	Result res = request.getHeaderValue("Content-Type");
	if (res.isOK() == true) {
		contentType = res.getOk();
	}
	std::string *elem = new(std::nothrow) std::string("CONTENT_TYPE=" + contentType);
	if (elem == NULL) { return false; }
	this->_env.push_back(elem->c_str());
	return true;
}

bool CGIHandler::setGateInterface(const Request &request, const std::string &actPath) {
	(void)actPath;
	std::string *elem = new(std::nothrow) std::string("GATEWAY_INTERFACE=CGI/1.1");
	if (elem == NULL) { return false; }
	this->_env.push_back(elem->c_str());
	return true;
}

bool CGIHandler::setPathInfo(const Request &request, const std::string &actPath) {
	std::string pathInfo;

	if (actPath.find(this->_scriptPath) == std::string::npos) {
		return false;
	}
	pathInfo = actPath.substr(this->_scriptPath.length());
	std::string::size_type posQuery = pathInfo.find("?");
	if (posQuery != std::string::npos) {
		pathInfo.erase(posQuery);
	}
	std::string *elem = new(std::nothrow) std::string("PATH_INFO=" + pathInfo);
	if (elem == NULL) { return false; }
	this->_env.push_back(elem->c_str());
	return true;
}

static std::string const removeLocationFromString(std::string const &path,
												  std::string const &location) {
	std::string result = path;
	size_t pos = result.find(location);
	if (pos != std::string::npos) {
		result.erase(pos, location.length());
	}
	return result;
}

bool CGIHandler::setPathTranslated(const Request &request, const std::string &actPath) {
	std::string pathInfo;
	std::string pathTranslated;

	if (actPath.find(this->_scriptPath) == std::string::npos) {
		return false;
	}
	pathInfo = actPath.substr(this->_scriptPath.length());
	std::string::size_type posQuery = pathInfo.find("?");
	if (posQuery != std::string::npos) {
		pathInfo.erase(posQuery);
	}
	if (pathInfo.empty() == false) {
		Location *location = this->_server->getLocationPointer(pathInfo);
		if (location != NULL) {
			if (location->getAliasDirective().empty() == false) {
				pathTranslated = location->getAliasDirective() + removeLocationFromString(pathInfo, location->getLocationPath());;
			} else {
				pathTranslated = location->getRoot() + pathInfo;
			}
		} else {
			pathTranslated = this->_server->getRoot() + pathInfo;
		}
	}
	std::string *elem = new(std::nothrow) std::string("PATH_TRANSLATED=" + pathTranslated);
	if (elem == NULL) { return false; }
	this->_env.push_back(elem->c_str());
	return true;
}

bool CGIHandler::setQueryString(const Request &request, const std::string &actPath) {
	std::string query;
	std::string originalPath(request.getPath());
	
	std::string::size_type posQuery = originalPath.find("?");
	if (posQuery != std::string::npos) {
		query = originalPath.substr(posQuery);
	}
	std::string *elem = new(std::nothrow) std::string("QUERY_STRING=" + query);
	if (elem == NULL) { return false; }
	this->_env.push_back(elem->c_str());
	return true;
}

bool CGIHandler::setRemoteAddr(const Request &request, const std::string &actPath) {
	std::string *elem = new(std::nothrow) std::string("REMOTE_ADDR=" + request.getRemoteAddr());
	if (elem == NULL) { return false; }
	this->_env.push_back(elem->c_str());
	return true;
}

bool CGIHandler::setRemoteHost(const Request &request, const std::string &actPath) {
	std::string *elem = new(std::nothrow) std::string("REMOTE_HOST=");
	if (elem == NULL) { return false; }
	this->_env.push_back(elem->c_str());
	return true;
}

bool CGIHandler::setRemoteMethod(const Request &request, const std::string &actPath) {
	std::string *elem = new(std::nothrow) std::string("REQUEST_METHOD=" + request.getMethod());
	if (elem == NULL) { return false; }
	this->_env.push_back(elem->c_str());
	return true;
}

bool CGIHandler::setScriptName(const Request &request, const std::string &actPath){
	std::string scriptName(request.getPath());
	std::string fileName;

	std::string::size_type posSlash = this->_scriptPath.find_last_of('/');
	if (posSlash == std::string::npos) { return false; }
	fileName = this->_scriptPath.substr(posSlash);
	scriptName.erase(scriptName.find(fileName) + fileName.length());
	
	std::string *elem = new(std::nothrow) std::string("SCRIPT_NAME=" + scriptName);
	if (elem == NULL) { return false; }
	this->_env.push_back(elem->c_str());
	return true;
}

bool CGIHandler::setServerName(const Request &request, const std::string &actPath) {
	std::stringstream ss(this->_server->getListen());
	std::string ipAddr;

	std::getline(ss, ipAddr, ':');
	std::string *elem = new(std::nothrow) std::string("SERVER_NAME=" + ipAddr);
	if (elem == NULL) { return false; }
	this->_env.push_back(elem->c_str());
}

bool CGIHandler::setServerPort(const Request &request, const std::string &actPath) {
	std::stringstream ss(this->_server->getListen());
	std::string port;

	std::getline(ss, port, ':');
	std::getline(ss, port);
	std::string *elem = new(std::nothrow) std::string("SERVER_PORT=" + port);
	if (elem == NULL) { return false; }
	this->_env.push_back(elem->c_str());
}

bool CGIHandler::setServerProtocol(const Request &request, const std::string &actPath) {
	std::string *elem = new(std::nothrow) std::string("SERVER_PROTOCOL=" + request.getHttpVersion());
	if (elem == NULL) { return false; }
	this->_env.push_back(elem->c_str());
}

bool CGIHandler::setServerSoftware(const Request &request, const std::string &actPath) {
void CGIHandler::setRuntimePath(const std::string &runtimePath) {
	this->_runtimePath = runtimePath;
}

void CGIHandler::setScriptPath(const std::string &scriptPath) {
	this->_scriptPath = scriptPath;
}
