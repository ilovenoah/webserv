#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <limits.h>

#include <map>
#include <string>

#include "CaseInsensitiveCompare.hpp"
#include "ClientSocket.hpp"
#include "Error.hpp"
#include "Ok.hpp"
#include "Result.hpp"
#include "utils.hpp"

class Request {
	public:
		enum rqphase { RQLINE, RQHEADER, RQBODY, RQFIN };

	private:
		std::string _method;
		std::string _path;
		std::string _httpVersion;
		std::map<std::string, std::string, CaseInsensitiveCompare> _header;
		std::string _body;
		Request::rqphase _phase;
		std::size_t _chunksize;
		std::string _remoteAddr;

	public:
		Request(const std::string &remoteAddr);
		void init();
		void setReqphase(Request::rqphase const rqphase);
		Request::rqphase getReqphase() const;
		void setMethod(std::string const &method);
		std::string const &getMethod() const;
		void setPath(std::string const &path);
		std::string const &getPath() const;
		void setHttpVersion(std::string const &httpVersion);
		void addHeader(std::string const &key, std::string const &value);
		std::string const &getHttpVersion() const;
		Result<std::string, bool> getHeaderValue(std::string const &key) const;
		std::string const &getBody() const;
		std::string const &getRemoteAddr() const;
		bool shouldKeepAlive() const;
		bool isValidRequest() const;
		ClientSocket::csphase load(std::stringstream &buffer);
		std::string getEntireData() const;
};

#endif
