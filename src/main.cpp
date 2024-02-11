#include "main.hpp"

static bool startUpServerSockets(std::map<int, ServerSocket> &ssmap, Config &config) {
	std::map<std::string, std::map<std::string, Server> > servers(config.getServers());
	for (std::map<std::string, std::map<std::string, Server> >::const_iterator iter = servers.begin(); iter != servers.end(); ++iter) {
		std::string ipAddr;
		std::string port;
		std::stringstream ss(iter->first);
		
		std::getline(ss, ipAddr, ':');
		std::getline(ss, port);
		ServerSocket svs(ipAddr, port);
		if (svs.init() == false) { return false; }
		std::cout << "Start up server: " << ipAddr << ":" << port << std::endl;
		ssmap.insert(std::pair<int, ServerSocket>(svs.getFd(), svs));
	}
	return true;
}

int main(int argc, char *argv[]) {
	Config conf;
	std::map<int, ServerSocket> ssmap;
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
	if (startUpServerSockets(ssmap, conf) == false) {
		return 1;
	}
	if (loop(ssmap, conf) == false) {
		return 1;
	}
}