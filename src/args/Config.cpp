#include "Config.hpp"
#include "Error.hpp"
#include "parse.hpp"

Config::Config()
{
	std::map<std::string, std::function<void()>> setterMap;

	setterMap["server_name"] = &Server::setServerName;
	setterMap["listen"] = &Server::setListen;
	setterMap["root"] = &Server::setRoot;
	setterMap["error_page"] = &Server::setErrorPage;
	setterMap["allow_methods"] = &Server::setAllowMethods;
	setterMap["index"] = &Server::setIndex;
	setterMap["client_body_limit"] = &Server::setClientBodyLimit;
	setterMap["location"] = &Server::setLocation;
	setterMap["error_page"] = &Server::setErrorPage;
}

static bool isComment(std::string& line)
{
	if (line[0] == '#')
		return (true);
	return (false);
}

static void deleteTab(std::string& line)
{
	size_t pos;

	while ((pos = line.find('\t')) != std::string::npos)
		line.erase(pos, 1);
}

void parseServer(Config config, std::string &line)

static void parseLine(Config config, std::string &line)
{
	size_t pos;

	if ((pos = line.find("server")) != std::string::npos)
	{
		config.add_back(Server());
	}
	else
	{
		key = line.substr(0, line.find(' '));
		value = line.substr(line.find(' ') + 1);
		config._servers.back().setterMap[key](value);
	}
}

static void parseFile(Config config, std::ifstream &file)
{
	std::string line;

	while (getline(file, line))
	{
		deleteTab(line);
		if (isComment(line))
			continue ;
		parseLine(config, line);
		}
	}
}

static void read_file(Config config, std::string path)
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
