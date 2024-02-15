#include "Response.hpp"

std::map<std::string, std::pair<std::string, std::string> > Response::_initErrorStatusMap() {
	std::map<std::string, std::pair<std::string, std::string> >  errorStatusMap;
// use macro for body
	errorStatusMap.insert(std::pair<std::string, std::pair<std::string, std::string> >("400", std::pair<std::string, std::string>("Bad Request", "Bad Request")));
	errorStatusMap.insert(std::pair<std::string, std::pair<std::string, std::string> >("403", std::pair<std::string, std::string>("Forbidden", "Forbidden")));
	errorStatusMap.insert(std::pair<std::string, std::pair<std::string, std::string> >("404", std::pair<std::string, std::string>("Not Found", "Not Found")));
	errorStatusMap.insert(std::pair<std::string, std::pair<std::string, std::string> >("500", std::pair<std::string, std::string>("Internal Server Error", "Internal Server Error")));
	return errorStatusMap;
}

std::map<std::string, std::pair<std::string, std::string> > Response::_errorStatusMap = _initErrorStatusMap();

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

void Response::setStatus(std::string const &status) {
	this->_status = status;
}

void Response::setStatusMsg(std::string const &statusMsg) {
	this->_statusMsg = statusMsg;
}

void Response::setBody(std::string const &body) {
	this->_body = body;
}

void Response::printConfigInfo() const {
	std::clog << "============== Routing result ==============" << std::endl;
	std::clog << "Server name: " << this->_server->getServername() << std::endl;
	if (this->_location != NULL) {
		std::clog << "Location path: " << this->_location->getLocationPath()
				  << std::endl;
	}
	std::clog << "Actual path: " << this->_actPath << std::endl;
	std::clog << "============================================" << std::endl;
}

void Response::_setErrorResponse(const std::string &status) {
	std::string errorPagePath;
	std::string root;
	std::string localRelativePath;
	std::fstream fs;
	std::size_t length;

	if (this->_location != NULL) {
		std::map<std::string, std::string>::const_iterator iter = this->_location->getErrorPages().find(status);
		if (iter != this->_location->getErrorPages().end()) {
			errorPagePath = iter->second;
		}
	} else {
		std::map<std::string, std::string>::const_iterator iter = this->_server->getErrorPages().find(status);
		if (iter != this->_server->getErrorPages().end()) {
			errorPagePath = iter->second;
		} else {
			this->_httpVersion = "HTTP/1.1";
			this->_status = status;
			this->_statusMsg = this->_errorStatusMap.find(status)->second.first;
			this->_body = this->_errorStatusMap.find(status)->second.second;
			this->_headers.insert(std::pair<std::string, std::string>(
				"Content-Length", utils::sizeTtoString(this->_body.size())));
			return ;
		}
	}
	if (this->_location->getRoot().empty() == false) {
		root = this->_location->getRoot();
	} else {
		root = this->_server->getRoot();
	}
	localRelativePath = root + errorPagePath;
	fs.open(localRelativePath.c_str());
	if (fs.fail() == true) {
		this->_httpVersion = "HTTP/1.1";
		this->_status = "500";
		this->_statusMsg = this->_errorStatusMap.find("500")->second.first;
		this->_body = this->_errorStatusMap.find("500")->second.second;
		this->_headers.insert(std::pair<std::string, std::string>(
			"Content-Length", utils::sizeTtoString(this->_body.size())));
		return ;
	} else {
		fs.seekg(0, fs.end);
		length = fs.tellg();
		fs.seekg(0, fs.beg);
		char buf[length];
		std::memset(buf, 0, length);
		fs.readsome(buf, length);
		if (fs.fail()) {
			this->_httpVersion = "HTTP/1.1";
			this->_status = "500";
			this->_statusMsg = this->_errorStatusMap.find("500")->second.first;
			this->_body = this->_errorStatusMap.find("500")->second.second;
			this->_headers.insert(std::pair<std::string, std::string>(
				"Content-Length", utils::sizeTtoString(this->_body.size())));
			return ;
		}
		this->_body.append(buf, length);
		this->_httpVersion = "HTTP/1.1";
		this->_status = status;
		this->_statusMsg = this->_errorStatusMap.find(status)->second.first;
		this->_headers.insert(std::pair<std::string, std::string>(
			"Content-Length", utils::sizeTtoString(this->_body.size())));
	}
}

ClientSocket::csphase Response::load(Config &config, Request const &request) {
	(void)config;
	(void)request;
	std::ifstream fs;
	std::size_t length(0);

	if (request.getMethod() == "GET") {
		if (utils::isAccess(this->_actPath, R_OK) == false) {
			this->_setErrorResponse("404");
			return ClientSocket::SEND;
		}
		Result<bool, std::string> res = utils::isDirectory(this->_actPath);
		if (res.isError()) {
			this->_setErrorResponse("500");
			return ClientSocket::SEND;
		}
		if (res.getOk() == true) {
			//directory listingへ
		}
		fs.open(this->_actPath.c_str(), std::ifstream::binary);
		if (fs.fail() == true) {
			this->_setErrorResponse("404");
			return ClientSocket::SEND;
		}
		fs.seekg(0, fs.end);
		length = fs.tellg();
		fs.seekg(0, fs.beg);
		char buf[length];
		std::memset(buf, 0, length);
		fs.readsome(buf, length);
		if (fs.fail()) {
			this->_setErrorResponse("500");
			return ClientSocket::SEND;
		}
		this->_body.append(buf, length);
		this->_httpVersion = "HTTP/1.1";
		this->_status = "200";
		this->_statusMsg = "Ok";
		this->_headers.insert(std::pair<std::string, std::string>(
			"Content-Length", utils::sizeTtoString(this->_body.size())));
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

static std::string const removeLocationFromString(std::string const &path, std::string const &location) {
	std::string result = path;
	size_t pos = result.find(location);
	if (pos != std::string::npos) {
		result.erase(pos, location.length());
	}
	return result;
}

void Response::setActPath(std::string const &path) {
	if (this->_location->getAliasDirective().empty() == false) {
		this->_actPath = this->_location->getAliasDirective() + removeLocationFromString(path, this->_location->getLocationPath());
	} else {
		if (this->_location->getRoot().empty() == true) {
			this->_actPath = this->_server->getRoot() + path;
		} else {
			this->_actPath = this->_location->getRoot() + path;
		}
	}
}

std::string const &Response::getActPath() const {
	return this->_actPath;
}