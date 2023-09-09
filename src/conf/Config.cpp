#include "Config.hpp"
#include "Error.hpp"
#include <iostream>

Config::Config()
{
}

Config::~Config()
{
}

Config::Config(const Config &copy)
{
	*this = copy;
}

Config &Config::operator=(const Config &copy)
{
	if (this != &copy)
		_servers = copy._servers;
	return *this;
}

void Config::setServers(int argc, const char *argv[])
{
	parseConfig(argc, argv);
}

void Config::setFilePath(std::string file_path)
{
	_filePath = file_path;
}

const std::vector<Server> &Config::getServers() const
{
	return _servers;
}

void Config::removeUnwanted(std::string &line)
{
	size_t pos;

	while ((pos = line.find('\t')) != std::string::npos)
		line.erase(pos, 1);
	if ((pos = line.find('#')) != std::string::npos)
		line.erase(pos);
	if ((pos = line.find('}')) != std::string::npos)
		line.erase(pos);
	if ((pos = line.find('\n')) != std::string::npos)
		line.erase(pos);
}

void Config::parseLine(std::string &line)
{
	size_t pos;
	std::string key;
	std::string value;

	if ((pos = line.find("server ")) != std::string::npos)
		_servers.push_back(Server());
	else
	{
		key = line.substr(0, line.find(' '));
		value = line.substr(line.find(' ') + 1, line.find(';') - line.find(' ') - 1);
		_servers.back().execSetterMap(key, value);
	}
}

void Config::parseFile(std::ifstream &file)
{
	std::string line;

	while (getline(file, line))
	{
		removeUnwanted(line);
		std::cout << line;
		if (line.empty())
			continue;
		std::cout << line.length() << std::endl;
		parseLine(line);
	}
}

void Config::readFile()
{
	std::ifstream file(_filePath.c_str());
	std::string line;

	try
	{
		if (file.fail())
			throw GenericException(0, "Failed to open file.");
		parseFile(file);
	}
	catch (GenericException &e)
	{
		std::cout << e.code() << ": " << e.what() << std::endl;
		std::exit(EXIT_FAILURE);
	}
}

void Config::parseConfig(int argc, const char *argv[])
{
	std::string file_path = DEFAULT_CONF;

	if (argc > 2)
	{
		std::cout << "Error: Too many arguments provided." << std::endl;
		std::cout << "Usage: ./webserv [config_file] or ./webserv" << std::endl;
		std::exit(EXIT_FAILURE);
	}
	if (argv[1])
		file_path = std::string(argv[1]);
	setFilePath(file_path);
	readFile();
}
