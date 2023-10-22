#include "HttpRequest.hpp"

HttpRequest::HttpRequest()
	: _server_software(SERVER_SOFTWARE), _gateway_interface(GATEWAY_INTERFACE), _content_length(0) {
}

HttpRequest::HttpRequest(std::string &remote_addr)
	: _server_software(SERVER_SOFTWARE), _gateway_interface(GATEWAY_INTERFACE), _remote_addr(remote_addr), _content_length(0) {
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
		_others = copy._others;
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

const std::map<std::string, std::string> &HttpRequest::getOthers() const {
	return _others;
}

std::size_t HttpRequest::getContentLength() {
	return _content_length;
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

bool HttpRequest::parseRequest(int fd) {
	std::string line;
	char buffer[BUFFER_SIZE];
	int bytes;

	if ((bytes = read(fd, buffer, BUFFER_SIZE - 1)) < 0) {
		return false;
	}
	buffer[bytes] = '\0';
	line = buffer;
	try {
		parseRequestLine(line);
		parseHeader(line);
		parseBody(line);
	} catch (GenericException &e) {
		return (false);
	}
	return (true);
}

// TODO: error handling
void HttpRequest::parseRequestLine(std::string &buffer) {
	if (_has_request_line) {
		return;
	}

	std::size_t pos;
	std::string headerLine;
	std::vector<std::string> contents;

	pos = buffer.find("\r\n");
	headerLine = buffer.substr(0, pos);
	contents = split(headerLine, " ");
	setRequestMethod(contents[0]);
	setQueryURI(contents[1]);
	setServerProtocol(contents[2]);
	_has_request_line = true;
	buffer.erase(0, pos);
}

void HttpRequest::parseHeader(std::string &buffer) {
	if (_has_header) {
		return;
	}

	std::size_t pos;
	std::vector<std::string> lines;

	buffer = _buffer + buffer;
	if ((pos = buffer.find("\r\n\r\n")) != std::string::npos) {
		if (pos + 4 < buffer.size()) {
			_body = buffer.substr(pos + 4);
			buffer.erase(pos);
			_has_body = true;
		}
		_has_header = true;
		lines = split(buffer, "\r\n");
		buffer.clear();
	} else {
		lines = split(buffer, "\r\n");
		_buffer = lines.back();
		lines.pop_back();
	}

	std::string key;
	std::string value;

	for (std::vector<std::string>::iterator it = lines.begin(); it != lines.end(); it++) {
		if ((pos = it->find(": ")) != std::string::npos) {
			key = it->substr(0, pos);
			value = it->substr(pos + 2);
			if (key == "Host") {
				pos = value.find(":");
				setServerName(value.substr(0, pos));
				setServerPort(value.substr(pos + 1));
			} else if (key == "Content-Length") {
				_content_length = atoi(value.c_str());
			} else if (key == "Transfer-Encoding") {
				if (value.find("chunked") != std::string::npos) {
					_is_chunked = true;
				}
			} else {
				_others[key] = value;
			}
		}
	}
}

void HttpRequest::parseBody(std::string &buffer) {
	if (!(_has_body && _is_chunked)) {
		return;
	}
	while (_is_chunked) {
		std::string len_str = buffer.substr(0, buffer.find("\r\n"));
		int len = atoi((len_str).c_str());
		
		std::cout << RED << "len: " << len_str << RESET << std::endl;
		if (len == 0) {
			_is_chunked = false;
			break;
		}
		std::string chunk = buffer.substr(buffer.find("\r\n") + 2, len);
		std::cout << RED << "chunk: " << chunk << RESET << std::endl;
		_body += chunk;
		buffer.erase(0, len + 2 + len_str.size());
	}
	if (_is_chunked) {
		_buffer = buffer;
	}
	if (_body.size() < _content_length) {
		_body += buffer;
	}
	if (_body.size() == _content_length) {
		_has_body = false;
	}
}
