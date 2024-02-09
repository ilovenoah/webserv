#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <string>
#include <fstream>
#include <sstream>
#include "Server.hpp"
#include "AConfigurable.hpp"

class Server;

class Location : public AConfigurable {
	private:
		std::string _path;
		std::size_t _lineCount;

	public:
		Location();
		bool setLocationPath(std::string const &attribute);
		const std::string &getLocationPath() const;
		void fillLocationDirectives(Server const &server);
};

#endif
