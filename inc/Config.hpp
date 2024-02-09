#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <sys/stat.h>
#include "Server.hpp"
#include "utils.hpp"
#include "errormsg.hpp"

class Server;

class Config {
	private:
		std::fstream _file;
		std::map<std::string, Server> _servers;
		Server _createServerInstance(std::fstream &file);
		static std::map<std::string, bool (Server::*)(const std::string&, std::fstream&)> _setterMap;
		static std::map<std::string, bool (Server::*)(const std::string&, std::fstream&)> initSetterMap();

	public:
		static std::size_t lineCount;
		bool open(char const *path);
		bool close();
		bool load();
		void printServers() const;
};


#endif