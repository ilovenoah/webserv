#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <sys/stat.h>

class Config {
	private:
		std::fstream _file;
		// std::vector<Server> _servers;

	public:
		bool open(char const *path);
		bool close();
		bool load();
};


#endif