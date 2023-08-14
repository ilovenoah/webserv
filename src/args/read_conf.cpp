#include "conf.hpp"
#include "Error.hpp"

void parseFile(std::map<std::string, Value> *config, std::ifstream& file)
{
	std::string line;
	(void)config;

	while (getline(file, line))
	{
		std::cout << "#" << line << std::endl;
	}
}

void read_file(std::map<std::string, Value> *config, std::string path)
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

std::map<std::string, Value> *parseConfig(int argc, const char *argv[])
{
	std::map<std::string, Value> *config;

	if (argc > 2)
	{
		std::cout << "Usage: ./webserv [config_file]" << std::endl;
		exit(EXIT_FAILURE);
	}
	read_file(config, argv[1]);
	return (config);
}
