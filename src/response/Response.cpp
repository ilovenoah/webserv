#include "Response.hpp"

std::map<std::string, std::pair<std::string, std::string> >
Response::_initstatusMap() {
	std::map<std::string, std::pair<std::string, std::string> > statusMap;
	statusMap.insert(
		std::pair<std::string, std::pair<std::string, std::string> >(
			"200", std::pair<std::string, std::string>("Ok", "Ok")));
	statusMap.insert(
		std::pair<std::string, std::pair<std::string, std::string> >(
			"201", std::pair<std::string, std::string>("Created", "Created")));
	statusMap.insert(
		std::pair<std::string, std::pair<std::string, std::string> >(
			"204",
			std::pair<std::string, std::string>("No Content", "No Content")));
	statusMap.insert(
		std::pair<std::string, std::pair<std::string, std::string> >(
			"301", std::pair<std::string, std::string>("Moved Permanently",
													   "Moved Permanently")));
	statusMap.insert(
		std::pair<std::string, std::pair<std::string, std::string> >(
			"302", std::pair<std::string, std::string>("Found", "Found")));
	statusMap.insert(
		std::pair<std::string, std::pair<std::string, std::string> >(
			"307", std::pair<std::string, std::string>("Temporary Redirect",
													   "Temporary Redirect")));
	statusMap.insert(
		std::pair<std::string, std::pair<std::string, std::string> >(
			"400",
			std::pair<std::string, std::string>("Bad Request", "Bad Request")));
	statusMap.insert(
		std::pair<std::string, std::pair<std::string, std::string> >(
			"403",
			std::pair<std::string, std::string>("Forbidden", "Forbidden")));
	statusMap.insert(
		std::pair<std::string, std::pair<std::string, std::string> >(
			"404",
			std::pair<std::string, std::string>("Not Found", "Not Found")));
	statusMap.insert(
		std::pair<std::string, std::pair<std::string, std::string> >(
			"405", std::pair<std::string, std::string>("Method Not Allowed",
													   "Method Not Allowed")));
	statusMap.insert(
		std::pair<std::string, std::pair<std::string, std::string> >(
			"413", std::pair<std::string, std::string>("Content Too Large",
													   "Content Too Large")));
	statusMap.insert(
		std::pair<std::string, std::pair<std::string, std::string> >(
			"500", std::pair<std::string, std::string>(
					   "Internal Server Error", "Internal Server Error")));
	return statusMap;
}

std::map<std::string, std::pair<std::string, std::string> >
	Response::_statusMap = _initstatusMap();

Response::Response()
	: _httpVersion("HTTP/1.1"), _server(NULL), _location(NULL) {}

void Response::setServerPointer(Config &config, Request const &request,
								std::string const &ipAddr,
								std::string const &port) {
	std::string listen(ipAddr + ":" + port);
	Result<std::string, bool> res(request.getHeaderValue("Host"));
	if (res.isError() == true) {
		this->_server = config.getDefaultServer(listen).getOk();
	} else {
		std::string hostName(res.getOk());
		this->_server = config.getServerPointer(listen, hostName);
	}
}

void Response::setLocationPointer(const std::string &path) {
	if (this->_server == NULL) {
		return;
	}
	this->_location = this->_server->getLocationPointer(path);
}

void Response::setHttpVersion(std::string const &httpVersion) {
	this->_httpVersion = httpVersion;
}

void Response::setStatus(std::string const &status) { this->_status = status; }

void Response::setStatusMsg(std::string const &statusMsg) {
	this->_statusMsg = statusMsg;
}

void Response::setBody(std::string const &body) { this->_body = body; }

bool Response::isKeepAlive() const {
	std::map<std::string, std::string>::const_iterator iter =
		this->_headers.find("Connection");
	if (iter == this->_headers.end()) {
		return true;
	}
	if (iter->second.compare("keep-alive") == 0) {
		return true;
	}
	return false;
}

bool Response::isCGIActive() const {
	return this->_cgiHandler.isActive();
}

void Response::printConfigInfo() const {
	std::clog << "============== Routing result ==============" << std::endl;
	std::clog << "Server name: ";
	{
		std::vector<std::string> defaultSeverNames =
			this->_server->getServernames();
		for (std::vector<std::string>::const_iterator dsiter =
				 defaultSeverNames.begin();
			 dsiter != defaultSeverNames.end(); ++dsiter) {
			std::clog << *dsiter << " ";
		}
	}
	std::clog << std::endl;
	if (this->_location != NULL) {
		std::clog << "Location path: " << this->_location->getLocationPath()
				  << std::endl;
	}
	std::clog << "Actual path: " << this->_actPath << std::endl;
	std::clog << "============================================" << std::endl;
}

void Response::_setErrorResponse(const std::string &status,
								 bool shouldKeepAlive) {
	std::string errorPagePath;
	std::string root;
	std::string localRelativePath;
	std::fstream fs;
	std::size_t length;

	if (this->_location != NULL) {
		std::map<std::string, std::string>::const_iterator iter =
			this->_location->getErrorPages().find(status);
		if (iter != this->_location->getErrorPages().end()) {
			errorPagePath = iter->second;
		}
	} else {
		std::map<std::string, std::string>::const_iterator iter =
			this->_server->getErrorPages().find(status);
		if (iter != this->_server->getErrorPages().end()) {
			errorPagePath = iter->second;
		} else {
			this->_setEntireDataWithBody(
				status, this->_statusMap.find(status)->second.second,
				shouldKeepAlive);
			return;
		}
	}
	if (this->_location != NULL &&
		this->_location->getRoot().empty() == false) {
		root = this->_location->getRoot();
	} else {
		root = this->_server->getRoot();
	}
	localRelativePath = root + errorPagePath;
	fs.open(localRelativePath.c_str());
	if (fs.fail() == true) {
		this->_setEntireDataWithBody(
			this->_statusMap.find("500")->first,
			this->_statusMap.find("500")->second.second, false);
		return;
	} else {
		fs.seekg(0, fs.end);
		length = fs.tellg();
		fs.seekg(0, fs.beg);
		char buf[length];
		std::memset(buf, 0, length);
		fs.readsome(buf, length);
		if (fs.fail()) {
			this->_setEntireDataWithBody(
				this->_statusMap.find("500")->first,
				this->_statusMap.find("500")->second.second, false);
			return;
		}
		std::string body(buf, length);
		this->_setEntireDataWithBody(status, body, shouldKeepAlive);
	}
}

static std::string getDirPath(const std::string &path) {
	std::string dirPath;
	std::size_t pos;
	if (path.compare("/") == 0 || path.compare("./") == 0) {
		return path;
	}
	pos = path.find_last_of('/');
	if (pos == std::string::npos) {
		return path;
	}
	dirPath = path.substr(0, pos + 1);
	return dirPath;
}

bool Response::_setIndexPage(bool shouldKeepAlive) {
	std::vector<std::string> index;
	std::ifstream fs;
	std::string path;
	if (this->_location != NULL) {
		index = this->_location->getIndex();
	} else {
		index = this->_server->getIndex();
	}
	for (std::vector<std::string>::iterator iter = index.begin();
		 iter != index.end(); ++iter) {
		path = this->_actPath + *iter;
		this->setEntireDataWithFile(path, "200", shouldKeepAlive);
		return true;
	}
	return false;
}

bool Response::_setDirectoryListingPage(const std::string &path,
										bool shouldKeepAlive) {
	std::string title("<html>\n<head><title>Index of " + path +
					  "</title></head>\n<body>\n");
	std::string head("<h1>Index of " + path +
					 "</h1><hr><pre>\n<a href=\"../\">../</a>\n");
	std::string aTagStart("<a href=\"");
	std::string aTagEnd("</a>");
	std::string data(title + head);

	DIR *dirp;
	struct dirent *dp;
	dirp = opendir(this->_actPath.c_str());
	if (dirp == NULL) {
		utils::putSysError("opendir");
		return false;
	}
	dp = readdir(dirp);
	while (dp != NULL) {
		if (std::strcmp(dp->d_name, ".") == 0 ||
			std::strcmp(dp->d_name, "..") == 0) {
			dp = readdir(dirp);
			continue;
		}
		data.append(aTagStart + dp->d_name + "\">" + dp->d_name + aTagEnd +
					"\n");
		dp = readdir(dirp);
	}
	if (closedir(dirp) == -1) {
		utils::putSysError("opendir");
		return false;
	}
	data.append("</pre><hr></body>\n</html>");
	this->_setEntireDataWithBody("200", data, shouldKeepAlive);
	return true;
}

ClientSocket::csphase Response::_setGetResponse(const Request &request) {
	if (utils::isAccess(this->_actPath, R_OK) == false) {
		this->_setErrorResponse("404", request.shouldKeepAlive());
		return ClientSocket::SEND;
	}
	Result<bool, std::string> res = utils::isDirectory(this->_actPath);
	if (res.isError()) {
		this->_setErrorResponse("500", false);
		return ClientSocket::SEND;
	}
	if (res.getOk() == true &&
		this->_actPath.find_last_of('/') != this->_actPath.length() - 1) {
		this->setEntireData("301", request.shouldKeepAlive());
		this->_headers.insert(std::pair<std::string, std::string>(
			"Location",
			"http://" + this->_server->getListen() + request.getPath() + "/"));
		return ClientSocket::SEND;
	}
	if (res.getOk() == true) {
		if (request.getPath().compare("/") == 0 ||
			(this->_location != NULL &&
			 request.getPath().compare(this->_location->getLocationPath() +
									   "/") == 0)) {
			if (this->_setIndexPage(request.shouldKeepAlive()) == true) {
				return ClientSocket::SEND;
			}
		}
		if (this->_shouldAutoIndexed() == true &&
			this->_setDirectoryListingPage(request.getPath(),
										   request.shouldKeepAlive()) == true) {
			return ClientSocket::SEND;
		}
		this->_setErrorResponse("404", request.shouldKeepAlive());
		return ClientSocket::SEND;
	}
	return this->setEntireDataWithFile(this->_actPath, "200",
									   request.shouldKeepAlive());
}

ClientSocket::csphase Response::_setPostResponse(const Request &request) {
	std::string uploadStorePath;
	std::string uploadPath;

	if (this->_location != NULL &&
		request.getBody().size() >
			(size_t)this->_location->getClientMaxBodySize()) {
		this->_setErrorResponse("413", false);
		return ClientSocket::SEND;
	} else if (request.getBody().size() >
			   (size_t)this->_server->getClientMaxBodySize()) {
		this->_setErrorResponse("413", false);
		return ClientSocket::SEND;
	}
	if (this->_location != NULL) {
		uploadStorePath = this->_location->getuploadStore() + "/";
	} else {
		uploadStorePath = this->_server->getuploadStore() + "/";
	}
	if (utils::isAccess(uploadStorePath, W_OK) == false) {
		this->_setErrorResponse("403", request.shouldKeepAlive());
		return ClientSocket::SEND;
	}
	uploadPath = uploadStorePath + utils::getRandomStr(12);
	while (utils::isAccess(uploadPath, F_OK) == true) {
		uploadPath = uploadStorePath + utils::getRandomStr(12);
	}
	std::ofstream ofs(uploadPath.c_str());
	if (ofs.fail() == true) {
		this->_setErrorResponse("500", false);
		return ClientSocket::SEND;
	}
	ofs.write(request.getBody().c_str(), request.getBody().size());
	if (ofs.fail() == true) {
		this->_setErrorResponse("500", false);
		return ClientSocket::SEND;
	}
	return this->setEntireData("201", request.shouldKeepAlive());
}

ClientSocket::csphase Response::_setDeleteResponse(const Request &request) {
	(void)request;
	if (utils::isAccess(this->_actPath, F_OK) == false) {
		this->_setErrorResponse("404", request.shouldKeepAlive());
		return ClientSocket::SEND;
	}
	std::string dirpath = getDirPath(this->_actPath);
	if (utils::isAccess(dirpath, W_OK) == false) {
		this->_setErrorResponse("403", request.shouldKeepAlive());
		return ClientSocket::SEND;
	}
	if (unlink(this->_actPath.c_str()) == -1) {
		this->_setErrorResponse("500", false);
		return ClientSocket::SEND;
	}
	return setEntireData("204", request.shouldKeepAlive());
}

bool Response::_shouldRedirect() const {
	if (this->_location != NULL &&
		this->_location->getReturn().empty() == false) {
		return true;
	} else if (this->_server->getReturn().empty() == false) {
		return true;
	}
	return false;
}

ClientSocket::csphase Response::_setRedirectResponse(Request const &request,
													 bool shouldKeepAlive) {
	if (this->_location != NULL) {
		this->_httpVersion = "HTTP/1.1";
		if (request.getMethod().compare("GET") == 0) {
			this->_status = "302";
			this->_statusMsg = this->_statusMap.find("302")->second.first;
		} else {
			this->_status = "307";
			this->_statusMsg = this->_statusMap.find("307")->second.first;
		}
		this->_headers.insert(std::pair<std::string, std::string>(
			"Content-Length", utils::sizeTtoString(this->_body.size())));
		this->_headers.insert(std::pair<std::string, std::string>(
			"Location", this->_location->getReturn()));
		if (shouldKeepAlive == true) {
			this->_headers.insert(std::pair<std::string, std::string>(
				"Connection", "keep-alive"));
		} else {
			this->_headers.insert(
				std::pair<std::string, std::string>("Connection", "close"));
		}
		return ClientSocket::SEND;
	} else {
		if (request.getMethod().compare("GET") == 0) {
			this->_status = "302";
			this->_statusMsg = this->_statusMap.find("302")->second.first;
		} else {
			this->_status = "307";
			this->_statusMsg = this->_statusMap.find("307")->second.first;
		}
		this->_headers.insert(std::pair<std::string, std::string>(
			"Content-Length", utils::sizeTtoString(this->_body.size())));
		this->_headers.insert(std::pair<std::string, std::string>(
			"Location", this->_server->getReturn()));
		if (shouldKeepAlive == true) {
			this->_headers.insert(std::pair<std::string, std::string>(
				"Connection", "keep-alive"));
		} else {
			this->_headers.insert(
				std::pair<std::string, std::string>("Connection", "close"));
		}
		return ClientSocket::SEND;
	}
	return ClientSocket::SEND;
}

void Response::_setCGIResponseHeader(const bool shouldKeepAlive) {
	std::stringstream ss(this->_cgiHandler.getRbuffer());
	std::string line;
	std::streampos startPos = ss.tellg();

	while (std::getline(ss, line) && line.compare("\r") != 0 && line.empty() == false) {
		line = utils::rmCR(line);
		std::stringstream hss(line);
		std::string key;
		std::string value;
		std::getline(hss, key, ':');
		hss >> std::ws;
		std::getline(hss, value);
		this->_headers.insert(std::pair<std::string, std::string>(key, value));
	}
	std::streampos endPos = ss.tellg();
	std::string::size_type readByte= endPos - startPos;
	this->_cgiHandler.eraseRbuffer(readByte);
	std::map<std::string, std::string>::const_iterator iter = this->_headers.find("Location");
	if (iter->second.find("/") == 0) {
		return ;
	}
	if (iter != this->_headers.end() && this->_headers.find("Content-Type") == this->_headers.end()) {
		this->_headers.insert(std::pair<std::string, std::string>("Connection", "close"));
		return ;
	}
	if (shouldKeepAlive == true) {
		this->_headers.insert(std::pair<std::string, std::string>("Connection", "keep-alive"));
		return ;
	}
	this->_headers.insert(std::pair<std::string, std::string>("Connection", "close"));
}

void Response::_setCGIResponseBody() {
	std::string body(this->_cgiHandler.getRbuffer());
	if (body.empty() != true) {
		this->_body = body;
		this->_headers.insert(std::pair<std::string, std::string>("Content-Length", utils::sizeTtoString(this->_body.size())));
	}
}

void Response::_setCGIResponseStatus() {
	std::map<std::string, std::string>::const_iterator lciter = this->_headers.find("Location");
	if (lciter == this->_headers.end()) {
		this->_status = "200";
		this->_statusMsg = "Ok";
		return ;
	}
	if (lciter->second.find_first_of('/') == 0) {
		return ;
	}
	std::map<std::string, std::string>::const_iterator ctiter = this->_headers.find("Content-Type");
	if (ctiter == this->_headers.end()) {
		this->_status = "302";
		this->_statusMsg = "Found";
		return ;
	}
	this->_status = "200";
	this->_statusMsg = "Ok";
}

bool Response::_isValidCGIResponse() const {
	if (this->_headers.find("Location") == this->_headers.end()) {
		if (this->_headers.find("Content-Type") == this->_headers.end()) {
			return false;
		}
		return true;
	}
	if (this->_body.empty() == false) {
		if (this->_headers.find("Content-Type") == this->_headers.end() || this->_headers.find("Status") == this->_headers.end()) {
			return false;
		}
	}
	return true;
}

bool Response::_isLocalRedirectResponse() {
	std::map<std::string, std::string>::const_iterator lriter = this->_headers.find("Location");
	 if (lriter == this->_headers.end() || this->_headers.size() != 1 || this->_body.empty() == false) {
		return false;
	}
	if (lriter->second.find("/") == 0) {
		return true;
	}
	return false;
}

ClientSocket::csphase Response::_setCGIResponse(Request &request ,bool shouldKeepAlive) {
	ClientSocket::csphase phase(ClientSocket::RECV);
	switch (this->_cgiHandler.detectCGIPhase())
	{
		case CGIHandler::CGIWRITE: {
			this->_cgiHandler.setCGIPhase(this->_cgiHandler.tryWrite());
			break;
		}
		case CGIHandler::CGIRECV: {
			this->_cgiHandler.setCGIPhase(this->_cgiHandler.tryRecv());
			break;
		}
		case CGIHandler::CGISET: {
			this->_setCGIResponseHeader(shouldKeepAlive);
			this->_setCGIResponseBody();
			this->_setCGIResponseStatus();
			if (this->_isValidCGIResponse() == false) {
				this->_headers.clear();
				this->_body.clear();
				this->setEntireData("500", false);
			}	
			if (this->_isLocalRedirectResponse() == true) {
				Result<std::string, bool> res = request.getHeaderValue("Host");
				request.init();
				request.setMethod("GET");
				std::map<std::string, std::string>::const_iterator lriter = this->_headers.find("Location");
				std::string path(lriter->second);
				request.setPath(path);
				request.setHttpVersion("HTTP/1.1");
				if (res.isOK() == true) {
					request.addHeader("Host", res.getOk());
				}
				request.setReqphase(Request::RQFIN);
				Server *serverPointer(this->_server);
				this->_clearResponse();
				this->_server = serverPointer;
				this->_httpVersion = "HTTP/1.1";
				this->setLocationPointer(path);
				this->setActPath(path);
				break;
			}
			this->_cgiHandler.setCGIPhase(CGIHandler::CGIFIN);
			break;
		}
		case CGIHandler::CGIFIN: {
			phase = ClientSocket::SEND;
			break;
		}
	}
	return phase;
}

bool Response::_shouldAutoIndexed() const {
	if (this->_location != NULL &&
		this->_location->getAutoindex() == AConfigurable::TRUE) {
		return true;
	} else if (this->_server->getAutoindex() == AConfigurable::TRUE) {
		return true;
	}
	return false;
}

bool Response::_shouldExecCGIScript() {
	std::string scriptPath(this->_actPath);
	std::map<std::string, std::string> cgiExtenstions;
	if (this->_location != NULL) {
		cgiExtenstions = this->_location->getCgiExtensions();
	} else {
		cgiExtenstions = this->_server->getCgiExtensions();
	}
	while (scriptPath.compare(".") != 0) {
		for (std::map<std::string, std::string>::const_iterator iter = cgiExtenstions.begin(); iter != cgiExtenstions.end(); ++iter) {
			std::size_t posExtension(scriptPath.find_last_of('.'));
			if (posExtension == std::string::npos) { return false; }
			if (scriptPath.substr(posExtension).compare(iter->first) == 0 && utils::isAccess(scriptPath, X_OK) == true && utils::isAccess(iter->second, X_OK)) {
				this->_cgiHandler.setRuntimePath(iter->second);
				this->_cgiHandler.setScriptPath(scriptPath);
				return true;
			}
		}
		std::size_t posSlash(scriptPath.find_last_of('/'));
		if (posSlash == std::string::npos) { return false; }
		scriptPath.erase(posSlash);
	}
	return false;
}

ClientSocket::csphase Response::load(Config &config, Request &request) {
	std::ifstream fs;

	(void)config;
	if (this->_cgiHandler.isActive() == true) {
		return this->_setCGIResponse(request, request.shouldKeepAlive());
	}
	if (request.isValidRequest() == false) {
		this->_setErrorResponse("400", false);
		return ClientSocket::SEND;
	}
	if (this->_location != NULL &&
		this->_location->isAllowedMethod(request.getMethod()) == false) {
		this->_setErrorResponse("405", request.shouldKeepAlive());
		return ClientSocket::SEND;
	} else if (this->_location == NULL && this->_server->isAllowedMethod(request.getMethod()) == false) {
		this->_setErrorResponse("405", request.shouldKeepAlive());
		return ClientSocket::SEND;
	}
	if (this->_shouldRedirect() == true) {
		return this->_setRedirectResponse(request, request.shouldKeepAlive());
	}
	if (this->_shouldExecCGIScript() == true) {
		if (this->_cgiHandler.init(request, *(this->_server), this->_actPath) == false) {
			this->_setErrorResponse("500", false);
			return ClientSocket::SEND;
		}
		if (this->_cgiHandler.activate() == false) {
			this->_setErrorResponse("500", false);
			return ClientSocket::SEND;
		}
		return ClientSocket::RECV;
	}
	if (request.getMethod() == "GET") {
		return this->_setGetResponse(request);
	} else if (request.getMethod() == "POST") {
		return this->_setPostResponse(request);
	} else if (request.getMethod() == "DELETE") {
		return this->_setDeleteResponse(request);
	}
	return ClientSocket::SEND;
}

std::string Response::getEntireData() const {
	std::string entireData;
	entireData += this->_httpVersion + " " + this->_status + " " +
				  this->_statusMsg + "\r\n";
	for (std::map<std::string, std::string>::const_iterator iter =
			 this->_headers.begin();
		 iter != this->_headers.end(); ++iter) {
		entireData += iter->first + ": " + iter->second + "\r\n";
	}
	entireData += "\r\n";
	entireData += this->_body;
	return entireData;
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

void Response::setActPath(std::string const &path) {
	std::string root;
	std::string	pathUnderRoot;

	if (this->_location == NULL) {
		root = this->_server->getRoot();
		pathUnderRoot = path;
	} else if (this->_location->getAliasDirective().empty() == false) {
		root = this->_location->getAliasDirective();
		pathUnderRoot = removeLocationFromString(path, this->_location->getLocationPath());
	} else if (this->_location->getRoot().empty() == true) {
		root = this->_server->getRoot();
		pathUnderRoot = path;
	} else {
		root = this->_location->getRoot();
		pathUnderRoot = path;
	}
	std::string::size_type posSlash = root.find_last_of('/');
	if (posSlash != std::string::npos && posSlash + 1 == root.length()) {
		root.erase(posSlash, 1);
	}
	this->_actPath = root + path;
}

std::string const &Response::getActPath() const { return this->_actPath; }

ClientSocket::csphase Response::setEntireDataWithFile(std::string const &path,
													  std::string const &status,
													  bool shouldKeepAlive) {
	std::ifstream fs;
	std::size_t length(0);

	if (utils::isAccess(path.c_str(), R_OK) == false) {
		this->_setErrorResponse("404", shouldKeepAlive);
		return ClientSocket::SEND;
	}
	fs.open(path.c_str(), std::ifstream::binary);
	if (fs.fail()) {
		this->_setErrorResponse("500", shouldKeepAlive);
		return ClientSocket::SEND;
	}
	fs.seekg(0, fs.end);
	length = fs.tellg();
	fs.seekg(0, fs.beg);
	char buf[length];
	std::memset(buf, 0, length);
	fs.readsome(buf, length);
	if (fs.fail()) {
		this->_setErrorResponse("500", shouldKeepAlive);
		return ClientSocket::SEND;
	}
	std::string body(buf, length);
	this->_setEntireDataWithBody(status, body, shouldKeepAlive);
	return ClientSocket::SEND;
}

ClientSocket::csphase Response::setEntireData(std::string const &status,
											  bool shouldKeepAlive) {
	this->_httpVersion = "HTTP/1.1";
	this->_status = status;
	this->_statusMsg = this->_statusMap.find(status)->second.first;
	this->_headers.insert(std::pair<std::string, std::string>(
		"Content-Length", utils::sizeTtoString(this->_body.size())));
	if (shouldKeepAlive == true) {
		this->_headers.insert(
			std::pair<std::string, std::string>("Connection", "keep-alive"));
	} else {
		this->_headers.insert(
			std::pair<std::string, std::string>("Connection", "close"));
	}
	return ClientSocket::SEND;
}

ClientSocket::csphase Response::_setEntireDataWithBody(
	std::string const &status, std::string const &body, bool shouldKeepAlive) {
	this->_httpVersion = "HTTP/1.1";
	this->_status = status;
	this->_body = body;
	this->_statusMsg = this->_statusMap.find(status)->second.first;
	this->_headers.insert(std::pair<std::string, std::string>(
		"Content-Length", utils::sizeTtoString(this->_body.size())));
	if (shouldKeepAlive == true) {
		this->_headers.insert(
			std::pair<std::string, std::string>("Connection", "keep-alive"));
	} else {
		this->_headers.insert(
			std::pair<std::string, std::string>("Connection", "close"));
	}
	return ClientSocket::SEND;
}

CGIHandler &Response::getCgiHandler() {
	return this->_cgiHandler;
}

void Response::_clearResponse() {
	this->_cgiHandler.clear();
	this->_httpVersion.clear();
	this->_status.clear();
	this->_statusMsg.clear();
	this->_headers.clear();
	this->_body.clear();
	this->_actPath.clear();
	this->_server = NULL;
	this->_location = NULL;
}