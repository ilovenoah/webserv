#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <dirent.h>
#include <sys/types.h>
#define HTTP_VERSION "HTTP/1.1"
#define STATUS_OK "200"
#define STATUS_CREATED "201"

#include <map>
#include <string>

#include "CGIHandler.hpp"
#include "ClientSocket.hpp"
#include "Config.hpp"
#include "Location.hpp"
#include "Request.hpp"
#include "Result.hpp"
#include "Server.hpp"
#include "utils.hpp"

class Response {
	private:
		CGIHandler _cgiHandler;
		std::string _httpVersion;
		std::string _status;
		std::string _statusMsg;
		std::map<std::string, std::string, CaseInsensitiveCompare> _headers;
		std::string _body;
		std::string _rawData;
		Server *_server;
		Location *_location;
		std::string _actPath;
		static std::map<std::string, std::pair<std::string, std::string> >
			_statusMap;
		static std::map<std::string, std::pair<std::string, std::string> >
		_initstatusMap();

		void _setErrorResponse(const std::string &status, bool shouldKeepAlive);
		bool _setIndexPage(bool shouldKeepAlive);
		bool _setDirectoryListingPage(const std::string &path,
									  bool shouldKeepAlive);
		ClientSocket::csphase _setGetResponse(const Request &request);
		ClientSocket::csphase _setPostResponse(const Request &request);
		ClientSocket::csphase _setDeleteResponse(const Request &request);
		bool _shouldRedirect() const;
		ClientSocket::csphase _setRedirectResponse(const Request &request,
												   bool shouldKeepAlive);
		ClientSocket::csphase _setCGIResponse(Request &request,
											  bool shouldKeepAlive);
		void _setCGIResponseHeader(const bool shouldKeepAlive);
		void _setCGIResponseBody();
		void _setCGIResponseStatus();
		bool _isLocalRedirectResponse();
		bool _isValidCGIResponse() const;
		bool _shouldAutoIndexed() const;
		bool _shouldExecCGIScript();
		ClientSocket::csphase _setEntireDataWithBody(std::string const &status,
													 std::string const &body,
													 bool shouldKeepAlive);
		void _clearResponse();

	public:
		Response();
		void setServerPointer(Config &config, Request const &request,
							  std::string const &ipAddr,
							  std::string const &port);
		void setLocationPointer(const std::string &path);
		void setHttpVersion(std::string const &httpVersion);
		void setStatus(std::string const &status);
		void setStatusMsg(std::string const &statusMsg);
		void setBody(std::string const &body);
		void setRawData();
		void eraseRawData(ssize_t bytes);
		bool isKeepAlive() const;
		bool isCGIActive() const;
		void printConfigInfo() const;
		ClientSocket::csphase load(Config &config, Request &request);
		std::string getEntireData() const;
		const std::string &getRawData() const;
		void setActPath(std::string const &path);
		std::string const &getActPath() const;
		ClientSocket::csphase setEntireDataWithFile(std::string const &path,
													std::string const &status,
													bool shouldKeepAlive);
		ClientSocket::csphase setEntireData(std::string const &status,
											bool shouldKeepAlive);
		CGIHandler &getCgiHandler();
};
#endif