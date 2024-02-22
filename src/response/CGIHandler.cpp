#include "CGIHandler.hpp"


std::vector<bool (CGIHandler::*)(const Request &, const std::string &)> CGIHandler::_initMetaVarSetterVec() {
	std::vector<bool (CGIHandler::*)(const Request &, const std::string &)> metaVarSetterVec;

	metaVarSetterVec.push_back(&CGIHandler::setAuthType);
	metaVarSetterVec.push_back(&CGIHandler::setContentLength);
	metaVarSetterVec.push_back(&CGIHandler::setContentType);
	metaVarSetterVec.push_back(&CGIHandler::setGateInterface);
	metaVarSetterVec.push_back(&CGIHandler::setPathInfo);
	metaVarSetterVec.push_back(&CGIHandler::setPathTranslated);
	metaVarSetterVec.push_back(&CGIHandler::setQueryString);
	metaVarSetterVec.push_back(&CGIHandler::setRemoteAddr);
	metaVarSetterVec.push_back(&CGIHandler::setRemoteHost);
	metaVarSetterVec.push_back(&CGIHandler::setRemoteMethod);
	metaVarSetterVec.push_back(&CGIHandler::setScriptName);
	metaVarSetterVec.push_back(&CGIHandler::setServerName);
	metaVarSetterVec.push_back(&CGIHandler::setServerPort);
	metaVarSetterVec.push_back(&CGIHandler::setServerProtocol);
	metaVarSetterVec.push_back(&CGIHandler::setServerSoftware);
	return metaVarSetterVec;
}

std::vector<bool (CGIHandler::*)(const Request &, const std::string &)> CGIHandler::_metaVarSetterVec = _initMetaVarSetterVec();

CGIHandler::CGIHandler()
	: _server(NULL), _request(NULL), _isActive(false), _revents(0), _wpfd(0), _rpfd(0) {}


static char *strDupToCharPtr(std::string const &src) {
	std::size_t i_str(0);
	char *str = new(std::nothrow) char[src.length() + 1]();
	if (str == NULL) {
		utils::putSysError("new");
		return NULL;
	}
	std::memset(str, 0, src.length() + 1);
	for (std::string::const_iterator iter = src.begin(); iter != src.end(); ++iter) {
		str[i_str] = *iter;
		i_str++;
	}
	return str;
}

bool CGIHandler::setAuthType(const Request &request, const std::string &actPath) {
	(void)actPath;
	std::string authType;
	Result<std::string, bool> res = request.getHeaderValue("Authorization");
	if (res.isOK() == true) {
		authType = res.getOk();
	}
	authType =  "AUTH_TYPE=" + authType;
	char *authTypePtr = strDupToCharPtr(authType);
	if (authTypePtr == NULL) { return false; }
	this->_env.push_back(authTypePtr);
	return true;
}

bool CGIHandler::setContentLength(const Request &request, const std::string &actPath) {
	(void)actPath;
	std::string contenLength;
	Result<std::string, bool> res = request.getHeaderValue("Content-Length");
	if (res.isOK() == true) {
		contenLength = res.getOk();
	}
	contenLength = "CONTENT_LENGTH=" + contenLength;
	char *contenLengthPtr = strDupToCharPtr(contenLength);
	if (contenLengthPtr == NULL) { return false; }
	this->_env.push_back(contenLengthPtr);
	return true;
}

bool CGIHandler::setContentType(const Request &request, const std::string &actPath) {
	(void)actPath;
	std::string contentType;
	Result<std::string, bool> res = request.getHeaderValue("Content-Type");
	if (res.isOK() == true) {
		contentType = res.getOk();
	}
	contentType = "CONTENT_TYPE=" + contentType;
	char *contentTypePtr = strDupToCharPtr(contentType);
	if (contentTypePtr == NULL) { return false; }
	this->_env.push_back(contentTypePtr);
	return true;
}

bool CGIHandler::setGateInterface(const Request &request, const std::string &actPath) {
	(void)actPath;
	(void)request;

	std::string gateWayInterface("GATEWAY_INTERFACE=CGI/1.1");
	char *gateWayInterfacePtr = strDupToCharPtr(gateWayInterface);
	if (gateWayInterfacePtr == NULL) { return false; }
	this->_env.push_back(gateWayInterfacePtr);
	return true;
}

bool CGIHandler::setPathInfo(const Request &request, const std::string &actPath) {
	(void)request;
	std::string pathInfo;

	if (actPath.find(this->_scriptPath) == std::string::npos) {
		return false;
	}
	pathInfo = actPath.substr(this->_scriptPath.length());
	std::string::size_type posQuery = pathInfo.find("?");
	if (posQuery != std::string::npos) {
		pathInfo.erase(posQuery);
	}
	pathInfo = "PATH_INFO=" + pathInfo;
	char *pathInfoPtr = strDupToCharPtr(pathInfo);
	if (pathInfoPtr == NULL) { return false; }
	this->_env.push_back(pathInfoPtr);
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
	(void)request;
	std::string pathInfo;
	std::string pathTranslated;

	if (actPath.find(this->_scriptPath) == std::string::npos) {
		return false;
	}
	pathInfo = actPath.substr(this->_scriptPath.length());
	if (pathInfo.compare("/") != 0) {
		std::string::size_type posQuery = pathInfo.find("?");
		if (posQuery != std::string::npos) {
			pathInfo.erase(posQuery);
		}
		if (pathInfo.empty() == false) {
			Location *location = this->_server->getLocationPointer(pathInfo);
			if (location != NULL) {
				if (location->getAliasDirective().empty() == false) {
					pathTranslated = location->getAliasDirective() + removeLocationFromString(pathInfo, location->getLocationPath());
				} else {
					pathTranslated = location->getRoot() + pathInfo;
				}
			} else {
				pathTranslated = this->_server->getRoot() + pathInfo;
			}
		}
	}
	pathTranslated = "PATH_TRANSLATED=" + pathTranslated;
	char *pathTranslatedPtr = strDupToCharPtr(pathTranslated);
	if (pathTranslatedPtr == NULL) { return false; }
	this->_env.push_back(pathTranslatedPtr);
	return true;
}

bool CGIHandler::setQueryString(const Request &request, const std::string &actPath) {
	(void)actPath;
	std::string query;
	std::string originalPath(request.getPath());
	
	std::string::size_type posQuery = originalPath.find("?");
	if (posQuery != std::string::npos) {
		query = originalPath.substr(posQuery+1);
	}
	query = "QUERY_STRING=" + query;
	char *queryPtr = strDupToCharPtr(query);
	if (queryPtr == NULL) { return false; }
	this->_env.push_back(queryPtr);
	return true;
}

bool CGIHandler::setRemoteAddr(const Request &request, const std::string &actPath) {
	(void)actPath;
	std::string remoteAddr("REMOTE_ADDR=" + request.getRemoteAddr());
	char *remoteAddrPtr = strDupToCharPtr(remoteAddr);
	if (remoteAddrPtr == NULL) { return false; }
	this->_env.push_back(remoteAddrPtr);
	return true;
}

bool CGIHandler::setRemoteHost(const Request &request, const std::string &actPath) {
	(void)actPath;
	(void)request;
	std::string remoteHost("REMOTE_HOST=");
	char *remoteHostPtr = strDupToCharPtr(remoteHost);
	if (remoteHostPtr == NULL) { return false; }
	this->_env.push_back(remoteHostPtr);
	return true;
}

bool CGIHandler::setRemoteMethod(const Request &request, const std::string &actPath) {
	(void)actPath;
	std::string remoteMethod("REQUEST_METHOD=" + request.getMethod());
	char *remoteMethodPtr = strDupToCharPtr(remoteMethod);
	if (remoteMethodPtr == NULL) { return false; }
	this->_env.push_back(remoteMethodPtr);
	return true;
}

bool CGIHandler::setScriptName(const Request &request, const std::string &actPath){
	(void)actPath;
	std::string scriptName(request.getPath());
	std::string fileName;

	std::string::size_type posSlash = this->_scriptPath.find_last_of('/');
	if (posSlash == std::string::npos) { return false; }
	fileName = this->_scriptPath.substr(posSlash);
	scriptName.erase(scriptName.find(fileName) + fileName.length());
	scriptName = "SCRIPT_NAME=" + scriptName;
	char *scriptNamePtr = strDupToCharPtr(scriptName);
	if (scriptNamePtr == NULL) { return false; }
	this->_env.push_back(scriptNamePtr);
	return true;
}

bool CGIHandler::setServerName(const Request &request, const std::string &actPath) {
	(void)request;
	(void)actPath;
	std::stringstream ss(this->_server->getListen());
	std::string ipAddr;

	std::getline(ss, ipAddr, ':');
	std::string serverName("SERVER_NAME=" + ipAddr);
	char *serverNamePtr = strDupToCharPtr(serverName);
	if (serverNamePtr == NULL) { return false; }
	this->_env.push_back(serverNamePtr);
	return true;
}

bool CGIHandler::setServerPort(const Request &request, const std::string &actPath) {
	(void)request;
	(void)actPath;
	std::stringstream ss(this->_server->getListen());
	std::string port;

	std::getline(ss, port, ':');
	std::getline(ss, port);
	std::string serverPort("SERVER_PORT=" + port);
	char *serverPortPtr = strDupToCharPtr(serverPort);
	if (serverPortPtr == NULL) { return false; }
	this->_env.push_back(serverPortPtr);
	return true;
}

bool CGIHandler::setServerProtocol(const Request &request, const std::string &actPath) {
	(void)actPath;
	std::string serverProtocol("SERVER_PROTOCOL=" + request.getHttpVersion());
	char *serverProtocolPtr = strDupToCharPtr(serverProtocol);
	if (serverProtocolPtr == NULL) { return false; }
	this->_env.push_back(serverProtocolPtr);
	return true;
}

bool CGIHandler::setServerSoftware(const Request &request, const std::string &actPath) {
	(void)actPath;
	(void)request;
	std::string serverSoftware("SERVER_SOFTWARE=webserv/1.0");
	char *serverSoftwarePtr = strDupToCharPtr(serverSoftware);
	if (serverSoftwarePtr == NULL) { return false; }
	this->_env.push_back(serverSoftwarePtr);
	return true;
}

bool CGIHandler::init(Request &request, Server &server, std::string const &actPath) {
	this->_request = &request;
	this->_server = &server;
	for (std::vector<bool (CGIHandler::*)(const Request &, const std::string &)>::iterator iter = this->_metaVarSetterVec.begin(); iter != this->_metaVarSetterVec.end(); ++iter) {
		if ((this->*(*iter))(request, actPath) == false) {
			// error handling
			return false;
		}
	}
	for (std::vector<const char *>::iterator iter = this->_env.begin(); iter != this->_env.end(); ++iter) {
		std::clog << *iter << std::endl;
	}
bool CGIHandler::deleteEnv() {
	for (std::vector<const char *>::iterator iter = this->_env.begin(); iter != this->_env.end();++iter) {
		delete [] *iter;
	}
	return true;
}

void CGIHandler::setRuntimePath(const std::string &runtimePath) {
	this->_runtimePath = runtimePath;
}

void CGIHandler::setScriptPath(const std::string &scriptPath) {
	this->_scriptPath = scriptPath;
}

// bool CGIHandler::activate(std::string const &cgiScriptURI, std::string const &runTimePath) {

// }