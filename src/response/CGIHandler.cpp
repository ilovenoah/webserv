#include "CGIHandler.hpp"

std::vector<bool (CGIHandler::*)(const Request &, const std::string &)>
CGIHandler::_initMetaVarSetterVec() {
	std::vector<bool (CGIHandler::*)(const Request &, const std::string &)>
		metaVarSetterVec;

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

std::vector<bool (CGIHandler::*)(const Request &, const std::string &)>
	CGIHandler::_metaVarSetterVec = _initMetaVarSetterVec();

const std::size_t CGIHandler::_timeoutSec(5);

CGIHandler::CGIHandler()
	: _server(NULL),
	  _request(NULL),
	  _isActive(false),
	  _revents(0),
	  _wpfd(0),
	  _rpfd(0),
	  _phase(CGIHandler::CGIWRITE),
	  _startSec(std::time(NULL)),
	  _exeTime(0) {}

bool CGIHandler::_deleteEnv() {
	for (std::vector<const char *>::iterator iter = this->_env.begin();
		 iter != this->_env.end(); ++iter) {
		delete[] * iter;
	}
	return true;
}

static char *strDupToCharPtr(std::string const &src) {
	std::size_t i_str(0);
	char *str = new (std::nothrow) char[src.length() + 1]();
	if (str == NULL) {
		utils::putSysError("new");
		return NULL;
	}
	std::memset(str, 0, src.length() + 1);
	for (std::string::const_iterator iter = src.begin(); iter != src.end();
		 ++iter) {
		str[i_str] = *iter;
		i_str++;
	}
	return str;
}

bool CGIHandler::setAuthType(const Request &request,
							 const std::string &actPath) {
	(void)actPath;
	std::string authType;
	Result<std::string, bool> res = request.getHeaderValue("Authorization");
	if (res.isOK() == true) {
		authType = res.getOk();
	}
	authType = "AUTH_TYPE=" + authType;
	char *authTypePtr = strDupToCharPtr(authType);
	if (authTypePtr == NULL) {
		return false;
	}
	this->_env.push_back(authTypePtr);
	return true;
}

bool CGIHandler::setContentLength(const Request &request,
								  const std::string &actPath) {
	(void)actPath;
	std::string contenLength;
	Result<std::string, bool> res = request.getHeaderValue("Content-Length");
	if (res.isOK() == true) {
		contenLength = res.getOk();
	}
	contenLength = "CONTENT_LENGTH=" + contenLength;
	char *contenLengthPtr = strDupToCharPtr(contenLength);
	if (contenLengthPtr == NULL) {
		return false;
	}
	this->_env.push_back(contenLengthPtr);
	return true;
}

bool CGIHandler::setContentType(const Request &request,
								const std::string &actPath) {
	(void)actPath;
	std::string contentType;
	Result<std::string, bool> res = request.getHeaderValue("Content-Type");
	if (res.isOK() == true) {
		contentType = res.getOk();
	}
	contentType = "CONTENT_TYPE=" + contentType;
	char *contentTypePtr = strDupToCharPtr(contentType);
	if (contentTypePtr == NULL) {
		return false;
	}
	this->_env.push_back(contentTypePtr);
	return true;
}

bool CGIHandler::setGateInterface(const Request &request,
								  const std::string &actPath) {
	(void)actPath;
	(void)request;

	std::string gateWayInterface("GATEWAY_INTERFACE=CGI/1.1");
	char *gateWayInterfacePtr = strDupToCharPtr(gateWayInterface);
	if (gateWayInterfacePtr == NULL) {
		return false;
	}
	this->_env.push_back(gateWayInterfacePtr);
	return true;
}

bool CGIHandler::setPathInfo(const Request &request,
							 const std::string &actPath) {
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
	if (pathInfoPtr == NULL) {
		return false;
	}
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

bool CGIHandler::setPathTranslated(const Request &request,
								   const std::string &actPath) {
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
					pathTranslated = location->getAliasDirective() +
									 removeLocationFromString(
										 pathInfo, location->getLocationPath());
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
	if (pathTranslatedPtr == NULL) {
		return false;
	}
	this->_env.push_back(pathTranslatedPtr);
	return true;
}

bool CGIHandler::setQueryString(const Request &request,
								const std::string &actPath) {
	(void)actPath;
	std::string query;
	std::string originalPath(request.getPath());

	std::string::size_type posQuery = originalPath.find("?");
	if (posQuery != std::string::npos) {
		query = originalPath.substr(posQuery + 1);
	}
	query = "QUERY_STRING=" + query;
	char *queryPtr = strDupToCharPtr(query);
	if (queryPtr == NULL) {
		return false;
	}
	this->_env.push_back(queryPtr);
	return true;
}

bool CGIHandler::setRemoteAddr(const Request &request,
							   const std::string &actPath) {
	(void)actPath;
	std::string remoteAddr("REMOTE_ADDR=" + request.getRemoteAddr());
	char *remoteAddrPtr = strDupToCharPtr(remoteAddr);
	if (remoteAddrPtr == NULL) {
		return false;
	}
	this->_env.push_back(remoteAddrPtr);
	return true;
}

bool CGIHandler::setRemoteHost(const Request &request,
							   const std::string &actPath) {
	(void)actPath;
	(void)request;
	std::string remoteHost("REMOTE_HOST=");
	char *remoteHostPtr = strDupToCharPtr(remoteHost);
	if (remoteHostPtr == NULL) {
		return false;
	}
	this->_env.push_back(remoteHostPtr);
	return true;
}

bool CGIHandler::setRemoteMethod(const Request &request,
								 const std::string &actPath) {
	(void)actPath;
	std::string remoteMethod("REQUEST_METHOD=" + request.getMethod());
	char *remoteMethodPtr = strDupToCharPtr(remoteMethod);
	if (remoteMethodPtr == NULL) {
		return false;
	}
	this->_env.push_back(remoteMethodPtr);
	return true;
}

bool CGIHandler::setScriptName(const Request &request,
							   const std::string &actPath) {
	(void)actPath;
	std::string scriptName(request.getPath());
	std::string fileName;

	std::string::size_type posSlash = this->_scriptPath.find_last_of('/');
	if (posSlash == std::string::npos) {
		return false;
	}
	fileName = this->_scriptPath.substr(posSlash);
	scriptName.erase(scriptName.find(fileName) + fileName.length());
	scriptName = "SCRIPT_NAME=" + scriptName;
	char *scriptNamePtr = strDupToCharPtr(scriptName);
	if (scriptNamePtr == NULL) {
		return false;
	}
	this->_env.push_back(scriptNamePtr);
	return true;
}

bool CGIHandler::setServerName(const Request &request,
							   const std::string &actPath) {
	(void)request;
	(void)actPath;
	std::stringstream ss(this->_server->getListen());
	std::string ipAddr;

	std::getline(ss, ipAddr, ':');
	std::string serverName("SERVER_NAME=" + ipAddr);
	char *serverNamePtr = strDupToCharPtr(serverName);
	if (serverNamePtr == NULL) {
		return false;
	}
	this->_env.push_back(serverNamePtr);
	return true;
}

bool CGIHandler::setServerPort(const Request &request,
							   const std::string &actPath) {
	(void)request;
	(void)actPath;
	std::stringstream ss(this->_server->getListen());
	std::string port;

	std::getline(ss, port, ':');
	std::getline(ss, port);
	std::string serverPort("SERVER_PORT=" + port);
	char *serverPortPtr = strDupToCharPtr(serverPort);
	if (serverPortPtr == NULL) {
		return false;
	}
	this->_env.push_back(serverPortPtr);
	return true;
}

bool CGIHandler::setServerProtocol(const Request &request,
								   const std::string &actPath) {
	(void)actPath;
	std::string serverProtocol("SERVER_PROTOCOL=" + request.getHttpVersion());
	char *serverProtocolPtr = strDupToCharPtr(serverProtocol);
	if (serverProtocolPtr == NULL) {
		return false;
	}
	this->_env.push_back(serverProtocolPtr);
	return true;
}

bool CGIHandler::setServerSoftware(const Request &request,
								   const std::string &actPath) {
	(void)actPath;
	(void)request;
	std::string serverSoftware("SERVER_SOFTWARE=webserv/1.0");
	char *serverSoftwarePtr = strDupToCharPtr(serverSoftware);
	if (serverSoftwarePtr == NULL) {
		return false;
	}
	this->_env.push_back(serverSoftwarePtr);
	return true;
}

bool CGIHandler::init(Request &request, Server &server,
					  std::string const &actPath) {
	extern char **environ;
	this->_request = &request;
	this->_server = &server;
	for (std::vector<bool (CGIHandler::*)(const Request &,
										  const std::string &)>::iterator iter =
			 this->_metaVarSetterVec.begin();
		 iter != this->_metaVarSetterVec.end(); ++iter) {
		if ((this->*(*iter))(request, actPath) == false) {
			// error handling
			this->_deleteEnv();
			return false;
		}
	}
	for (std::size_t i_env = 0; environ[i_env] != NULL; ++i_env) {
		char *elem;
		if (std::strncmp(environ[i_env], "PWD=", 4) == 0) {
			std::string pathScriptDir(this->_scriptPath);
			std::string::size_type posLastSlash =
				pathScriptDir.find_last_of('/');
			if (posLastSlash != std::string::npos) {
				pathScriptDir.erase(posLastSlash + 1);
			}
			elem = strDupToCharPtr("PWD=" + pathScriptDir);
		} else {
			elem = strDupToCharPtr(environ[i_env]);
		}
		if (elem == NULL) {
			this->_deleteEnv();
			return false;
		}
		this->_env.push_back(elem);
	}
#if defined(_DEBUG)
	std::clog << "============= CGI Metavariables =============" << std::endl;
	for (std::vector<const char *>::iterator iter = this->_env.begin();
		 iter != this->_env.end(); ++iter) {
		std::clog << *iter << std::endl;
	}
	std::clog << "=============================================" << std::endl;
#endif
	this->_wbuffer = this->_request->getBody();
	this->_env.push_back(NULL);
	return true;
}

bool CGIHandler::activate() {
	int ipfd[2];
	int opfd[2];
	std::string pathScriptDir(this->_scriptPath);
	std::string::size_type posLastSlash(pathScriptDir.find_last_of('/'));
	if (posLastSlash != std::string::npos) {
		pathScriptDir.erase(posLastSlash + 1);
	}
	const char *arg[3];
	arg[0] = this->_runtimePath.c_str();
	arg[1] = this->_scriptPath.c_str();
	arg[2] = NULL;
	std::memset(ipfd, 0, sizeof(ipfd));
	std::memset(opfd, 0, sizeof(opfd));
	if (utils::x_pipe(ipfd) == -1) {
		return false;
	}
	if (utils::x_pipe(opfd) == -1) {
		utils::x_close(ipfd[0]);
		utils::x_close(ipfd[1]);
		return false;
	}
	if (fcntl(ipfd[0], F_SETFL, O_NONBLOCK) == -1) {
		utils::putSysError("fcntl");
		utils::x_close(ipfd[0]);
		utils::x_close(ipfd[1]);
		utils::x_close(opfd[0]);
		utils::x_close(opfd[1]);
		return false;
	}
	if (fcntl(ipfd[1], F_SETFL, O_NONBLOCK) == -1) {
		utils::putSysError("fcntl");
		utils::x_close(ipfd[0]);
		utils::x_close(ipfd[1]);
		utils::x_close(opfd[0]);
		utils::x_close(opfd[1]);
		return false;
	}
	if (fcntl(opfd[0], F_SETFL, O_NONBLOCK) == -1) {
		utils::putSysError("fcntl");
		utils::x_close(ipfd[0]);
		utils::x_close(ipfd[1]);
		utils::x_close(opfd[0]);
		utils::x_close(opfd[1]);
		return false;
	}
	if (fcntl(opfd[1], F_SETFL, O_NONBLOCK) == -1) {
		utils::putSysError("fcntl");
		utils::x_close(ipfd[0]);
		utils::x_close(ipfd[1]);
		utils::x_close(opfd[0]);
		utils::x_close(opfd[1]);
		return false;
	}
	if (fcntl(ipfd[0], FD_CLOEXEC) == -1) {
		utils::putSysError("fcntl");
		utils::x_close(ipfd[0]);
		utils::x_close(ipfd[1]);
		utils::x_close(opfd[0]);
		utils::x_close(opfd[1]);
		return false;
	}
	if (fcntl(ipfd[1], FD_CLOEXEC) == -1) {
		utils::putSysError("fcntl");
		utils::x_close(ipfd[0]);
		utils::x_close(ipfd[1]);
		utils::x_close(opfd[0]);
		utils::x_close(opfd[1]);
		return false;
	}
	if (fcntl(opfd[0], FD_CLOEXEC) == -1) {
		utils::putSysError("fcntl");
		utils::x_close(ipfd[0]);
		utils::x_close(ipfd[1]);
		utils::x_close(opfd[0]);
		utils::x_close(opfd[1]);
		return false;
	}
	if (fcntl(opfd[1], FD_CLOEXEC) == -1) {
		utils::putSysError("fcntl");
		utils::x_close(ipfd[0]);
		utils::x_close(ipfd[1]);
		utils::x_close(opfd[0]);
		utils::x_close(opfd[1]);
		return false;
	}
	pid_t pid = fork();
	if (pid == -1) {
		utils::putSysError("fork");
		utils::x_close(ipfd[0]);
		utils::x_close(ipfd[1]);
		utils::x_close(opfd[0]);
		utils::x_close(opfd[1]);
		return false;
	}
	if (pid == 0) {
		if (utils::x_chdir(pathScriptDir.c_str()) == -1) {
			this->_deleteEnv();
			exit(EXIT_FAILURE);
		}
		if (utils::x_dup2(ipfd[0], 0) == -1) {
			this->_deleteEnv();
			exit(EXIT_FAILURE);
		}
		if (utils::x_close(ipfd[1]) == -1) {
			this->_deleteEnv();
			exit(EXIT_FAILURE);
		}
		if (utils::x_dup2(opfd[1], 1) == -1) {
			this->_deleteEnv();
			exit(EXIT_FAILURE);
		}
		if (utils::x_close(opfd[0]) == -1) {
			this->_deleteEnv();
			exit(EXIT_FAILURE);
		}
		execve(this->_runtimePath.c_str(), (char *const *)arg,
			   (char *const *)this->_env.data());
		utils::putSysError("execve");
		this->_deleteEnv();
		exit(EXIT_FAILURE);
	}
	if (utils::x_close(ipfd[0]) == -1) {
		this->_deleteEnv();
		utils::x_close(opfd[1]);
		utils::x_kill(pid, SIGTERM);
		return false;
	}
	if (utils::x_close(opfd[1]) == -1) {
		this->_deleteEnv();
		utils::x_kill(pid, SIGTERM);
		return false;
	}
	this->_deleteEnv();
	this->_wpfd = ipfd[1];
	this->_rpfd = opfd[0];
	this->_pid = pid;
	this->_isActive = true;
	this->_exeTime++;
	return true;
}

void CGIHandler::setRuntimePath(const std::string &runtimePath) {
	this->_runtimePath = runtimePath;
}

void CGIHandler::setScriptPath(const std::string &scriptPath) {
	this->_scriptPath = scriptPath;
}

const std::string &CGIHandler::getRbuffer() const { return this->_rbuffer; }

void CGIHandler::eraseRbuffer(std::string::size_type readBytes) {
	if (readBytes <= _rbuffer.size()) {
		_rbuffer.erase(0, readBytes);
	} else {
		_rbuffer.clear();
	}
}

bool CGIHandler::isActive() const { return this->_isActive; }

void CGIHandler::setRevents(const short revents) { this->_revents = revents; }

short CGIHandler::getRevents() const { return this->_revents; }

int CGIHandler::getMonitoredFd() const {
	int monitoredFd(0);
	switch (this->_phase) {
		case CGIHandler::CGIWRITE:
			monitoredFd = this->_wpfd;
			break;

		case CGIHandler::CGIRECV:
			monitoredFd = this->_rpfd;
			break;
		default:
			monitoredFd = this->_wpfd;
			break;
	}
	return monitoredFd;
}

pid_t CGIHandler::getPid() const { return this->_pid; }

CGIHandler::cgiphase CGIHandler::getCGIPhase() const { return this->_phase; }

void CGIHandler::setCGIPhase(CGIHandler::cgiphase phase) {
	this->_phase = phase;
}

CGIHandler::cgiphase CGIHandler::detectCGIPhase() const {
	CGIHandler::cgiphase phase(this->_phase);
	if (std::difftime(std::time(NULL), this->_startSec) > CGIHandler::_timeoutSec || this->_exeTime > CGI_LIMIT_LOCAL_REDIR) {
		utils::x_kill(this->_pid, SIGTERM);
		if (this->_phase == CGIHandler::CGIFIN) {
			return CGIHandler::CGIFIN;
		}
		return CGIHandler::CGISET;
	}
	pid_t waitpid = this->tryWait();
	switch (phase) {
		case CGIHandler::CGIWRITE: {
			if (waitpid == -1) {
				phase = CGIHandler::CGISET;
				break;
			}
			if (waitpid != 0) {
				phase = CGIHandler::CGIRECV;
				break;
			}
			break;
		}
		case CGIHandler::CGIRECV: {
			if (waitpid == -1) {
				phase = CGIHandler::CGISET;
				break;
			}
			if (waitpid != 0) {
				phase = CGIHandler::CGISET;
				break;
			}
			break;
		}
		default: {
			break;
		}
	}
	return phase;
}

CGIHandler::cgiphase CGIHandler::tryWrite() {
	if (this->_wbuffer.size() == 0) {
		return CGIHandler::CGIRECV;
	}
	if ((this->_revents & POLLOUT) != POLLOUT) {
		return CGIHandler::CGIWRITE;
	}
	ssize_t wlen =
		write(this->_wpfd, this->_wbuffer.c_str(), this->_wbuffer.size());
	if (wlen == -1) {
		utils::putSysError("write");
		return CGIHandler::CGISET;
	}
	if ((std::size_t)wlen < this->_wbuffer.size()) {
		this->_wbuffer.erase(0, wlen);
		return CGIHandler::CGIWRITE;
	}
	this->_wbuffer.clear();
	utils::x_close(this->_wpfd);
	return CGIHandler::CGIRECV;
}

CGIHandler::cgiphase CGIHandler::tryRecv() {
	if ((this->_revents & POLLIN) != POLLIN) {
		return CGIHandler::CGIWRITE;
	}
	char buf[CGI_BUFFERSIZE];
	std::memset(buf, 0, sizeof(buf));
	ssize_t rlen = read(this->_rpfd, buf, CGI_BUFFERSIZE - 1);
	if (rlen == -1) {
		utils::putSysError("read");
		return CGIHandler::CGISET;
	}
	if (rlen == 0) {
		utils::x_close(this->_rpfd);
		return CGIHandler::CGISET;
	}
	this->_rbuffer.append(buf, rlen);
	return CGIHandler::CGIRECV;
}

pid_t CGIHandler::tryWait() const {
	int status(0);
	return waitpid(this->_pid, &status, WNOHANG);
}

void CGIHandler::clear() {
	this->_server = NULL;
	this->_request = NULL;
	this->_env.clear();
	this->_scriptPath.clear();
	this->_runtimePath.clear();
	this->_wbuffer.clear();
	this->_rbuffer.clear();
	this->_isActive = false;
	this->_revents = 0;
	this->_wpfd = 0;
	this->_rpfd = 0;
	this->_rpfd = 0;
	this->_pid = 0;
	this->_phase = CGIHandler::CGIWRITE;
}