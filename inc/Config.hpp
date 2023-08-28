#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "Server.hpp"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>

class Config
{
  private:
	std::vector<Server> _servers;

  public:
	Config();
	~Config();
	Config(const Config &copy);
	Config &operator=(const Config &copy);

	void setServers();
	std::vector<Server> &getServers() const;
	Config parseConfig(int argc, const char *argv[]);
};

#endif
