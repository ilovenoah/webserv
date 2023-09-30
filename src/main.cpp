#include "main.hpp"


int main(int argc, const char *argv[]) {
	Config config;

	config.setServers(argc, argv);
	startServerLoop(config);
	return (0);
}
