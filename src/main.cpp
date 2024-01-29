#include "main.hpp"
#include "ServerSocket.hpp"
#include "loop.hpp"


// int main(int argc, const char *argv[]) {
// 	Config config;

// 	config.setServers(argc, argv);
// 	startServerLoop(config);
// 	return (0);
// }

int main() {
	ServerSocket ss("127.0.0.1", "8080");
	ss.init();
	std::map<int, ServerSocket> ssmap;
	ssmap.insert(std::pair<int, ServerSocket>(ss.getFd(), ss));
	bool test = loop(ssmap);
	(void)test;
}