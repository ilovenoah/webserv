#include "Config.hpp"
#include "Error.hpp"

Config::Config() {}

Config::~Config() {}

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

void Config::setServers()
{
	_servers.push_back(Server());
}

const std::vector<Server> &Config::getServers() const
{
	return _servers;
}

void removeUnwanted(std::string &line)
{
	size_t pos;

	while ((pos = line.find('\t')) != std::string::npos)
		line.erase(pos, 1);
	if ((pos = line.find('#')) != std::string::npos)
		line.erase(pos);
	if ((pos = line.find('}')) != std::string::npos)
		line.erase(pos);
}

void parseLine(Config config, std::string &line)
{
	size_t pos;
	std::string key;
	std::string value;

	if ((pos = line.find("server ")) != std::string::npos)
		config.setServers();
	else
	{
		key = line.substr(0, line.find(' '));
		value = line.substr(line.find(' ') + 1, line.find(';') - 1);
		config.getServers().back().execSetterMap(key, value);
	}
}

void parseFile(Config config, std::ifstream &file)
{
	std::string line;

	while (getline(file, line))
	{
		removeUnwanted(line);
		if (line.empty())
			continue;
		parseLine(config, line);
	}
}

void read_file(Config config, std::string path)
{
	std::ifstream file(path.c_str());
	std::string line;

	try
	{
		if (file.fail())
			throw GenericException(0, "Failed to open file.");
		parseFile(config, file);
	}
	catch (GenericException &e)
	{
		std::cout << e.code() << ":" << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}
}

Config parseConfig(int argc, const char *argv[])
{
	Config config;

	if (argc > 2)
	{
		std::cout << "Usage: ./webserv [config_file]" << std::endl;
		exit(EXIT_FAILURE);
	}
	read_file(config, argv[1]);
	return (config);
}
