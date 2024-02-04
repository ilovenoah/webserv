#include "main.hpp"
#include "ServerSocket.hpp"
#include "loop.hpp"


// int main(int argc, const char *argv[]) {
// 	Config config;

// 	config.setServers(argc, argv);
// 	startServerLoop(config);
// 	return (0);
// }

int main(int argc, const char *argv[]) {
	ServerSocket ss("127.0.0.1", "8080");
	Config config;
	config.setServers(argc, argv);
	ss.init();
	std::map<int, ServerSocket> ssmap;
	ssmap.insert(std::pair<int, ServerSocket>(ss.getFd(), ss));
	bool test = loop(ssmap, config);
	(void)test;
}