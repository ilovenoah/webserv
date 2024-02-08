#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <string>
#include <map>
#include "AConfigurable.hpp"
#include "Location.hpp"
#include "utils.hpp"

#define MIN_USERPORT 1024
#define MAX_USERPORT 49151

class Server : public AConfigurable {
	private:
		std::string _servername;
		std::string _ipAddr;
		std::string _port;
		std::string _root;
		std::map<std::string, Location> _locations;
		static std::map<std::string, bool (Location::*)(const std::string&, std::fstream&)> _setterMap;
		static std::map<std::string, bool (Location::*)(const std::string&, std::fstream&)> initSetterMap();

	public:
		Server();
		bool setServername(std::string const &attribute, std::fstream &file);
		const std::string &getServername() const;
		bool setListen(std::string const &attribute, std::fstream &file);
		std::string getListen() const;
		const std::string &getIpaddr() const;
		const std::string &getPort() const;
		bool setRoot(std::string const &attribute, std::fstream &file);
		const std::string &getRoot() const;
		bool setLocations(std::string const &attribute, std::fstream &file);
		const std::map<std::string, Location> &getLocations() const;
};

#endif