bool CGIHandler::setAuthType(const Request &request, const std::string &scriptPath) {
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

bool CGIHandler::setContentLength(const Request &request, const std::string &scriptPath) {
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
