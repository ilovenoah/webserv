#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <fstream>
#include <sstream>
#include <string>

#include "AConfigurable.hpp"
#include "Server.hpp"

class Server;

class Location : public AConfigurable {
	private:
		std::string _path;
		std::string _aliasDirective;

	public:
		Location();
		bool setLocationPath(std::string const &attribute);
		const std::string &getLocationPath() const;
		bool setAliasDirective(std::string const &attribute,
							   std::fstream &file);
		const std::string &getAliasDirective() const;
		bool setRoot(std::string const &attribute, std::fstream &file);
		void fillLocationDirectives(Server const &server);
};

#endif
