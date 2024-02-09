#include "main.hpp"

// static bool startUpServerSockets(std::map<int, ServerSocket> &ssmap, Config
// &config) { 	std::vector<Server> servers(config.getServers()); 	for
// (std::vector<Server>::iterator iter = servers.begin(); iter != servers.end();
// ++iter) { 		std::string ipAddr = iter->getIpAddress(); 		std::string
// port = iter->getPort(); 		if (ipAddr.empty() == true) { ipAddr =
// "0.0.0.0"; } if (port.empty() == true) { port = "8000"; } ServerSocket
// ss(ipAddr, port); 		if (ss.init() == false) { return false; }
// std::cout << "Start up server: "
// << ipAddr << ":" << port << std::endl; 		ssmap.insert(std::pair<int,
// ServerSocket>(ss.getFd(), ss));
// 	}
// 	return true;
// }

int main(int argc, char *argv[]) {
	Config conf;
	if (argc != 2) {
		std::cerr << "Usage: ./webserv config/filepath" << std::endl;
		return 1;
	}
	if (conf.open(argv[1]) == false) {
		return 1;
	}
	if (conf.load() == false) {
		return 1;
	}
	conf.printServers();
	if (conf.close() == false) {
		return 1;
	}
}