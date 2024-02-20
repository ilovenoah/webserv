#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include <string>
#include <map>

#include "Request.hpp"

class CGIHandler
{
	public:
		enum cgiphase { CGIEXEC, CGIWRITE, CGIRECV, CGIFIN };

	private:
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
		bool init(const Request &request, std::string const &scriptPath, std::string const &runtimePath);
		bool activate();
		bool isActive() const;
		void setRevents(const short revents);
		short getRevents() const;
		int getMonitoredFd() const;
		bool setAuthType(const Request &request, const std::string &scriptPath);
		bool setContentLength(const Request &request, const std::string &scriptPath);
		bool setContentType(const Request &request, const std::string &scriptPath);
		bool setGateInterface(const Request &request, const std::string &scriptPath);
		CGIHandler::cgiphase getCGIPhase() const;
};

#endif