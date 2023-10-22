#include "main.hpp"

int main(int argc, const char *argv[]) {
	Config config;

	config.setServers(argc, argv);

	// debug用
	HttpRequest req;
	int fd = open("sample.txt", O_RDONLY);
	for (int i = 0; i < 10; i++) {
		req.parseRequest(fd);
	}
	std::cout << "ServerSoftware  : " << "|" << req.getServerSoftware() << "|" << std::endl;
	std::cout << "ServerName      : " << "|" << req.getServerName() << "|" << std::endl;
	std::cout << "GatewayInterface: " << "|" << req.getGatewayInterface() << "|" << std::endl;
	std::cout << "ServerProtocol  : " << "|" << req.getServerProtocol() << "|" << std::endl;
	std::cout << "ServerPort      : " << "|" << req.getServerPort() << "|" << std::endl;
	std::cout << "RequestMethod   : " << "|" << req.getRequestMethod() << "|" << std::endl;
	std::cout << "PathInfo        : " << "|" << req.getPathInfo() << "|" << std::endl;
	std::cout << "PathTranslated  : " << "|" << req.getPathTranslated() << "|" << std::endl;
	std::cout << "ScriptName      : " << "|" << req.getScriptName() << "|" << std::endl;
	std::cout << "QueryString     : " << "|" << req.getQueryString() << "|" << std::endl;
	std::cout << "RemoteAddr      : " << "|" << req.getRemoteAddr() << "|" << std::endl;
	std::cout << "Body            : " << std::endl << "|" << req.getBody() << "|" << std::endl;
	return (0);

	startServerLoop(config);
	return (0);
}
