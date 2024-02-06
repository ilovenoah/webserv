#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <sys/stat.h>
#include "Server.hpp"

class Config {
	private:
		std::fstream _file;
		std::map<std::string, Server> _servers;

	public:
		bool open(char const *path);
		bool close();
		bool load();
};


#endif