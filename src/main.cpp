#include "main.hpp"

#include "ServerSocket.hpp"
#include "loop.hpp"

static bool startUpServerSockets(std::map<int, ServerSocket> &ssmap, Config &config) {
	std::vector<Server> servers(config.getServers());
	for (std::vector<Server>::iterator iter = servers.begin(); iter != servers.end(); ++iter) {
		std::string ipAddr = iter->getIpAddress();
		std::string port = iter->getPort();
		if (ipAddr.empty() == true) { ipAddr = "0.0.0.0"; }
		if (port.empty() == true) { port = "8000"; }
		ServerSocket ss(ipAddr, port);
		if (ss.init() == false) { return false; }
		ssmap.insert(std::pair<int, ServerSocket>(ss.getFd(), ss));
	}
	return true;
}

int main(int argc, const char *argv[]) {
	std::map<int, ServerSocket> ssmap;
	Config config;

	config.setServers(argc, argv);
	startUpServerSockets(ssmap, config);
	loop(ssmap, config);
}