#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <sys/types.h>
#include <dirent.h>
#define HTTP_VERSION "HTTP/1.1"
#define STATUS_OK "200"
#define STATUS_CREATED "201"
// #define STATUS_CREATED "201"

#include <map>
#include <string>

#include "ClientSocket.hpp"
#include "Config.hpp"
#include "Location.hpp"
#include "Request.hpp"
#include "Server.hpp"
#include "utils.hpp"
#include "Result.hpp"

class Response {
	private:
		std::string _httpVersion;
		std::string _status;
		std::string _statusMsg;
		std::map<std::string, std::string, CaseInsensitiveCompare> _headers;
		std::string _body;
		Server *_server;
		Location *_location;
		std::string _actPath;
		static std::map<std::string, std::pair<std::string, std::string> > _statusMap;
		static std::map<std::string, std::pair<std::string, std::string> > _initstatusMap();

		void _setErrorResponse(const std::string &status);
		bool _setIndexPage();
		bool _setDirectoryListingPage(const std::string &path);

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
		void printConfigInfo() const;
		ClientSocket::csphase load(Config &config, Request const &request);
		std::string getEntireData() const;
		void setActPath(std::string const &path);
		std::string const &getActPath() const;
		ClientSocket::csphase setEntireDataWithFile(std::string const &path, std::string const &status);
		ClientSocket::csphase setEntireData(std::string const &status);
};
#endif