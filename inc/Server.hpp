#ifndef SERVER_HPP
#define SERVER_HPP

#include <map>
#include <string>
#include <vector>

#include "AConfigurable.hpp"
#include "Config.hpp"
#include "Location.hpp"
#include "utils.hpp"

#define MIN_USERPORT 1024
#define MAX_USERPORT 49151

class Location;

class Server : public AConfigurable {
	private:
		std::string _servername;
		std::string _ipAddr;
		std::string _port;
		std::map<std::string, Location> _locations;
		static std::map<std::string,
						bool (Location::*)(const std::string &, std::fstream &)>
			_setterMap;
		static std::map<std::string,
						bool (Location::*)(const std::string &, std::fstream &)>
		initSetterMap();

	public:
		Server();
		bool setServername(std::string const &attribute, std::fstream &file);
		const std::string &getServername() const;
		bool setListen(std::string const &attribute, std::fstream &file);
		std::string getListen() const;
		const std::string &getIpaddr() const;
		const std::string &getPort() const;
		bool setLocations(std::string const &attribute, std::fstream &file);
		const std::map<std::string, Location> &getLocations() const;
		void fillLocationDirectives();
		Location *getLocationPointer(const std::string &originalPath);
};

#endif