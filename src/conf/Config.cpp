#include "Config.hpp"
#include "utils.hpp"

std::map<std::string, bool (Server::*)(std::string const&, std::fstream&)> Config::initSetterMap() {
	std::map<std::string, bool (Server::*)(const std::string&, std::fstream&)> srvSetterMap;
	srvSetterMap["server_name"] = &Server::setServername;
	// srvSetterMap["listen"] = &Server::setListen;
	// srvSetterMap["root"] = &Server::setRoot;
	// srvSetterMap["allow_methods"] = &Server::setAllowMethods;
	// srvSetterMap["autoindex"] = &Server::setAutoIndex;
	// srvSetterMap["index"] = &Server::setIndex;
	// srvSetterMap["client_body_limit"] = &Server::setClientBodyLimit;
	// srvSetterMap["cgi_info"] = &Server::setCgiInfo;
	// srvSetterMap["return"] = &Server::setReturn;
	// srvSetterMap["location"] = &Server::setLocation;
	// srvSetterMap["error_page"] = &Server::setErrorPage;
	return srvSetterMap;
}

std::map<std::string, bool (Server::*)(std::string const&, std::fstream&)> Config::_setterMap = initSetterMap();

bool Config::open(char const *path) {
	struct stat statbuf;
	
	if (stat(path, &statbuf) != 0) {
		utils::putSysError("stat");
		return false;
	}
	if (S_ISDIR(statbuf.st_mode) == true) {
		std::cerr << RED << "Webserv: Error: a file path is directory." << RESET << std::endl;
		return false;
	}
	this->_file.open(path, std::ios::in);
	if (this->_file.fail()) { 
		utils::putSysError("open");
		return false;
	}
	return true;
}

bool Config::close() {
	this->_file.close();
	if (this->_file.fail()) {
		utils::putSysError("close");
		return false;
	}
	return true;
}

static bool shouldIgnore(std::string const &line) {
	if (line.empty() == true) { return true; }
	std::stringstream ss(line);
	std::string elem;

	ss >> elem;
	if (elem.empty() == true) { return true; }
	if (elem[0] == '#') { return true; }
	return false;
}

Server Config::_createServerInstance(std::fstream &file, std::size_t lineCount) {
	std::string line;
	Server server;

	while (std::getline(file, line)) {
		lineCount++;
		if (shouldIgnore(line)) { continue; }
		if (line[line.size() - 1] != ';') { /* errorhandling; */ }
		if (line.size() != 0) { line = line.substr(0, line.size() - 1); }
		std::stringstream ss(line);
		std::string elem;
		ss >> elem;
		std::map<std::string, bool (Server::*)(std::string const&, std::fstream&)>::iterator iter = this->_setterMap.find(elem);
		if (iter == this->_setterMap.end()) { /* errorhandling; */ }
		if ((server.*(iter->second))(line, file) == false) { /* errorhandling; */ }
	}
	return server;
}

bool Config::load() {
	std::string line;
	std::size_t lineCount = 0;
	while (std::getline(this->_file, line)) {
		lineCount++;
		if (shouldIgnore(line)) { continue; }
		std::stringstream ss(line);
		std::string elem;
		ss >> elem;
		char bracket;
		ss >> bracket;
		if (ss.fail() || !ss.eof()) {
			// error handling	
		}
		if (elem.compare("server") == 0 && bracket == '{') {
			try {
				Server server = this->_createServerInstance(this->_file, lineCount);

			} catch (std::exception &e) {
				//
			}

		}
	}
	if (this->_servers.size() == 0) {
		std::cerr << RED << "Webserv: Error: no server is defined." << RESET << std::endl;
		return false;
	}
	return true;
}

void Config::printServers() const {
	for (std::map<std::string, Server>::const_iterator iter = this->_servers.begin(); iter != this->_servers.end(); ++iter) {
		std::clog << "====================================" << std::endl;
		std::clog << "Server name: " << iter->second.getServername() << std::endl;
		std::clog << "====================================" << std::endl;
	}
}

// int main(int argc, char *argv[]) {
// 	Config test;

// 	test.open(argv[1]);
// 	test.load();
// 	test.close();
// }