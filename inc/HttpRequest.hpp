#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include "env.hpp"

#define BUFFER_SIZE 10

#define SERVER_SOFTWARE "webserv/1.0"
#define GATEWAY_INTERFACE "CGI/1.1"

class HttpRequest {
   private:
	std::string _server_software;
	std::string _server_name;
	std::string _gateway_interface;
	std::string _server_protocol;
	std::string _server_port;
	std::string _request_method;
	std::string _path_info;
	std::string _path_translated;
	std::string _script_name;
	std::string _query_string;
	std::string _remote_addr;
	std::string _body;
	std::size_t _content_length;

	std::string _socket_time; // time out用

	std::string _buffer;
	std::map<std::string, std::string> _others;

	bool _has_request_line;
	bool _has_header;
	bool _has_body;
	bool _is_chunked;

   public:
	HttpRequest();
	HttpRequest(std::string &remote_addr);
	~HttpRequest();

	HttpRequest(const HttpRequest &other);
	HttpRequest &operator=(const HttpRequest &other);

	void setServerSoftware(const std::string &values);
	void setServerName(const std::string &values);
	void setGatewayInterface(const std::string &values);
	void setServerProtocol(const std::string &values);
	void setServerPort(const std::string &values);
	void setRequestMethod(const std::string &values);
	void setPathInfo(const std::string &values);
	void setPathTranslated(const std::string &values);
	void setScriptName(const std::string &values);
	void setQueryString(const std::string &values);
	void setRemoteAddr(const std::string &values);
	void setBody(const std::string &values);

	const std::string &getServerSoftware() const;
	const std::string &getServerName() const;
	const std::string &getGatewayInterface() const;
	const std::string &getServerProtocol() const;
	const std::string &getServerPort() const;
	const std::string &getRequestMethod() const;
	const std::string &getPathInfo() const;
	const std::string &getPathTranslated() const;
	const std::string &getScriptName() const;
	const std::string &getQueryString() const;
	const std::string &getRemoteAddr() const;
	const std::string &getBody() const;
	const std::map<std::string, std::string> &getOthers() const;
	std::size_t getContentLength();

	bool isGet();
	bool isPost();
	bool isDelete();

	bool parseRequest(int fd);
	void parseRequestLine(std::string &buffer);
	void parseHeader(std::string &buffer);
	void parseBody(std::string &buffer);
	void setQueryURI(const std::string &values);
};

#endif
