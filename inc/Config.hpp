#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <vector>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include "Server.hpp"

class Config
{
private:
	std::vector<Server> _servers;
	std::map<std::string, std::function<void(Server&)>> _srvSetterMap;

public:
	Config();
	~Config();
	Config(const Config &copy);
	Config &operator=(const Config &copy);

	void setServers(const std::vector<Server> &servers);
	const std::vector<Server> &getServers() const;
	Config parseConfig(int argc, const char *argv[]);

};

#endif
