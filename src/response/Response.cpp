#include "Response.hpp"

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

void Response::printConfigInfo() const {
	std::clog << "============== Routing result ==============" << std::endl;
	std::clog << "Server name: " << this->_server->getServername() << std::endl;
	if (this->_location != NULL) {
		std::clog << "Location path: " << this->_location->getLocationPath()
				  << std::endl;
	}
	std::clog << "============================================" << std::endl;
}

ClientSocket::csphase Response::load(Config &config, Request const &request) {
	(void)config;
	(void)request;
	std::string localRelativePath("." + request.getPath());

	if (request.getMethod() == "GET") {
		struct stat statbuf;

		if (stat(localRelativePath.c_str(), &statbuf) != 0) {
			utils::putSysError("stat");
			// error handling
		}
		if (S_ISDIR(statbuf.st_mode) == true) {
			// error handling
		}
		std::ifstream fs(localRelativePath.c_str(), std::ifstream::binary);
		if (fs.fail() == true) {
			// error handling
		}
		fs.seekg(0, fs.end);
		std::size_t length = fs.tellg();
		fs.seekg(0, fs.beg);

		char buf[length];
		std::memset(buf, 0, length);
		fs.readsome(buf, length);
		if (fs.fail()) {
			// error handling
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