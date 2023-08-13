#include "main.hpp"

int error_log(std::string string)
{
	std::ofstream log;
	log.open("error.log", std::ios::app);
	log << string << std::endl;
	log.close();
	std::cout << string << std::endl;
	return(1);
}

int main(int argc, char const *argv[])
{
	std::map <std::string, Value> config;

	if (argc > 2)
		return(error_log("Too many arguments"));
	(void)argv;

	config["server_name"].type = STRING;
	config["server_name"].value.strVal = new std::string("server_name");
	
	return (0);
}
