#include "main.hpp"

int main(int argc, const char *argv[])
{
	Config config;

	config.parseConfig(argc, argv);

	// config読み込み
	// config["server_name"].type = STRING;
	// config["server_name"].value.strVal = new std::string("server_name");

	return (0);
}
