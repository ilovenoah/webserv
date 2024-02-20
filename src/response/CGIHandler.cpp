bool CGIHandler::setAuthType(const Request &request, const std::string &scriptPath) {
	std::string authType;
	Result res = request.getHeaderValue("Authorization");
	if (res.isOK() == true) {
		authType = res.getOk();
	}
	std::string *elem = new(std::nothrow) std::string("AUTH_TYPE=" + authType);
	if (elem == NULL) { return false; }
	env.push_back(elem->c_str());
	return true;
}

