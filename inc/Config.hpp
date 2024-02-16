#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <sys/stat.h>

#include <fstream>
#include <iostream>
#include <map>
#include <string>

#include "Error.hpp"
#include "Ok.hpp"
#include "Result.hpp"
#include "Server.hpp"
#include "errormsg.hpp"
#include "utils.hpp"

class Server;

class Config {
	private:
		std::fstream _file;
		std::map<std::string, std::map<std::string, Server> > _servers;
		std::map<std::string, Server *> _defaultServers;
		Server _createServerInstance(std::fstream &file);
		static std::map<std::string,
						bool (Server::*)(const std::string &, std::fstream &)>
			_setterMap;
		static std::map<std::string,
						bool (Server::*)(const std::string &, std::fstream &)>
		initSetterMap();

	public:
		static std::size_t lineCount;
		bool open(char const *path);
		bool close();
		bool load();
		void printServers() const;
		const Result<Server *, bool> getDefaultServer(
			const std::string &listen) const;
		const std::map<std::string, std::map<std::string, Server> >
			&getServers() const;
		Server *getServerPointer(const std::string &listen,
								 const std::string &hostName);
};

#endif