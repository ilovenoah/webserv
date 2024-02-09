#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <string>
#include <fstream>
#include <sstream>
#include "AConfigurable.hpp"

class Location : public AConfigurable {
	private:
		std::string _path;
		std::size_t _lineCount;

	public:
		Location();
		bool setLocationPath(std::string const &attribute);
		const std::string &getLocationPath() const;
};

#endif
