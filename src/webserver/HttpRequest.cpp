#include "HttpRequest.hpp"

HttpRequest::HttpRequest()
	: _server_software(SERVER_SOFTWARE), _gateway_interface(GATEWAY_INTERFACE) {
}

HttpRequest::HttpRequest(std::string &remote_addr)
	: _server_software(SERVER_SOFTWARE), _gateway_interface(GATEWAY_INTERFACE), _remote_addr(remote_addr) {
}

HttpRequest::~HttpRequest() {
}

HttpRequest::HttpRequest(const HttpRequest &copy) {
	*this = copy;
}

HttpRequest &HttpRequest::operator=(const HttpRequest &copy) {
	if (this != &copy) {
		_server_software = copy._server_software;
		_server_name = copy._server_name;
		_gateway_interface = copy._gateway_interface;
		_server_protocol = copy._server_protocol;
		_server_port = copy._server_port;
		_request_method = copy._request_method;
		_path_info = copy._path_info;
		_path_translated = copy._path_translated;
		_script_name = copy._script_name;
		_query_string = copy._query_string;
		_remote_addr = copy._remote_addr;
		_body = copy._body;
	}
	return *this;
}

void HttpRequest::setServerSoftware(const std::string &values) {
	_server_software = values;
}

void HttpRequest::setServerName(const std::string &values) {
	_server_name = values;
}

void HttpRequest::setGatewayInterface(const std::string &values) {
	_gateway_interface = values;
}

void HttpRequest::setServerProtocol(const std::string &values) {
	_server_protocol = values;
}

void HttpRequest::setServerPort(const std::string &values) {
	_server_port = values;
}

void HttpRequest::setRequestMethod(const std::string &values) {
	_request_method = values;
}

void HttpRequest::setPathInfo(const std::string &values) {
	_path_info = values;
}

void HttpRequest::setPathTranslated(const std::string &values) {
	_path_translated = values;
}

void HttpRequest::setScriptName(const std::string &values) {
	_script_name = values;
}

void HttpRequest::setQueryString(const std::string &values) {
	_query_string = values;
}

void HttpRequest::setRemoteAddr(const std::string &values) {
	_remote_addr = values;
}

void HttpRequest::setBody(const std::string &values) {
	_body = values;
}

const std::string &HttpRequest::getServerSoftware() const {
	return _server_software;
}

const std::string &HttpRequest::getServerName() const {
	return _server_name;
}

const std::string &HttpRequest::getGatewayInterface() const {
	return _gateway_interface;
}

const std::string &HttpRequest::getServerProtocol() const {
	return _server_protocol;
}

const std::string &HttpRequest::getServerPort() const {
	return _server_port;
}

const std::string &HttpRequest::getRequestMethod() const {
	return _request_method;
}

const std::string &HttpRequest::getPathInfo() const {
	return _path_info;
}

const std::string &HttpRequest::getPathTranslated() const {
	return _path_translated;
}

const std::string &HttpRequest::getScriptName() const {
	return _script_name;
}

const std::string &HttpRequest::getQueryString() const {
	return _query_string;
}

const std::string &HttpRequest::getRemoteAddr() const {
	return _remote_addr;
}

const std::string &HttpRequest::getBody() const {
	return _body;
}

bool HttpRequest::isGet() {
	return _request_method == "GET";
}

bool HttpRequest::isPost() {
	return _request_method == "POST";
}

bool HttpRequest::isDelete() {
	return _request_method == "DELETE";
}

// TODO: 2個目を分解
void HttpRequest::setQueryURI(const std::string &values) {
	std::vector<std::string> lines;

	lines = split(values, "?");
	if (lines.size() == 2) {
		setPathInfo(lines[0]);
		// PATH_TRANSLATED
		// SCRIPT_NAME
		setQueryString(lines[1]);
	} else {
		setPathInfo(values);
		// PATH_TRANSLATED
		// SCRIPT_NAME
	}
}

bool HttpRequest::parseHeader(int fd) {
	std::string request;
	bool endFlag = false;
	char buffer[BUFFER_SIZE];
	int bytes;

	if ((bytes = read(fd, buffer, BUFFER_SIZE - 1)) < 0) {
		return false;
	}
	buffer[bytes] = '\0';
	request = buffer;

	if (request.find("\r\n\r\n") != std::string::npos) {
		endFlag = true;
	}

	std::string key;
	std::string value;
	std::size_t pos;
	std::vector<std::string> lines;
	std::vector<std::string> headers;

	lines = split(request, "\r\n");
	if (_buffer.size() > 0) {
		lines[0] = _buffer + lines[0];
		_buffer.clear();
	}
	for (std::vector<std::string>::iterator it = lines.begin(); it != lines.end(); it++) {
		if ((pos = it->find(": ")) != std::string::npos) {
			key = it->substr(0, pos);
			value = it->substr(pos + 2);
			if (key == "Host") {
				pos = value.find(":");
				setServerName(value.substr(0, pos));
				setServerPort(value.substr(pos + 1));
			}
		} else {
			std::string methodList[3] = {"GET", "POST", "DELETE"};
			headers = split(*it, " ");
			if (std::find(std::begin(methodList), std::end(methodList), headers[0])) {
				std::cout << GREEN << "pass" << RESET << std::endl;
				setRequestMethod(headers[0]);
				setQueryURI(headers[1]);
				setServerProtocol(headers[2]);
			}
		}
	}
	if (endFlag) {
		setBody(request.substr(request.find("\r\n\r\n") + 4));
	}
	return true;
}
