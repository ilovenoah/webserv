#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include <string>
#include <map>

#include "Request.hpp"
#include "Server.hpp"

class CGIHandler
{
	public:
		enum cgiphase { CGIEXEC, CGIWRITE, CGIRECV, CGIFIN };

	private:
		Server *_server;
		std::vector<const char *> _env;
		std::string _scriptPath;
		std::string _runtimePath;
		bool _isActive;
		short _revents;
		int _wpfd;
		int _rpfd;
		static std::vector<bool (CGIHandler::*)(const Request &, const std::string &)> _initMetaVarSetterVec();
		static std::vector<bool (CGIHandler::*)(const Request &, const std::string &)> _metaVarSetterVec;

	public:
		bool init(const Request &request, const Server *server, std::string const &actPath, std::string const &runtimePath);
		bool activate();
		bool isActive() const;
		void setRevents(const short revents);
		short getRevents() const;
		int getMonitoredFd() const;
		bool setAuthType(const Request &request, const std::string &actPath);
		bool setContentLength(const Request &request, const std::string &actPath);
		bool setContentType(const Request &request, const std::string &actPath);
		bool setGateInterface(const Request &request, const std::string &actPath);
		bool setPathInfo(const Request &request, const std::string &actPath);
		bool setPathTranslated(const Request &request, const std::string &actPath);
		bool setQueryString(const Request &request, const std::string &actPath);
		CGIHandler::cgiphase getCGIPhase() const;
};

#endif