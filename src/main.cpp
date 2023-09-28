#include "main.hpp"

#include "CreateSocket.hpp"
#include "ServerLoop.hpp"

int main(int argc, const char *argv[]) {
	Config config;

	config.setServers(argc, argv);
	startServerLoop(config);
	return (0);
}
