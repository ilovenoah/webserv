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
		Server _createServerInstance(std::fstream &file, std::size_t lineCoun);
		static std::map<std::string, bool (Server::*)(const std::string&, std::fstream&)> _setterMap;
		static std::map<std::string, bool (Server::*)(const std::string&, std::fstream&)> initSetterMap();


	public:
		bool open(char const *path);
		bool close();
		bool load();
};


#endif