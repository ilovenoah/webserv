#include "Config.hpp"
#include "Error.hpp"
#include "parse.hpp"

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

static void parseFile(Config config, std::ifstream& file)
{
	std::string line;

	while (getline(file, line))
	{
		deleteTab(line);
		if (isComment(line))
			continue ;
		// 一番右が"{"ならば左側がキー、"}"までvalue
		if (line.find('{') != std::string::npos)
		{
			// "}"までがvalue
		}
		// "{"がなければ始めのスペースまでがキー
		else
		{
			// 最初のスペースまでがキー
			// その後がvalue
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
