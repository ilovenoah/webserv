#include "Config.hpp"
#include "utils.hpp"

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

bool Config::load() {
	std::string line;
	while (std::getline(this->_file, line)){
		if (shouldIgnore(line)) { continue; }
		std::clog << "hello" << std::endl;
		// std::stringstream ss(line);
		// std::string elem;
		// char bracket;
		// ss >> elem >> bracket;
		// if (ss.fail() || !ss.eof()) {
			
		// }
		// if (elem )
	}
	if (this->_servers.size() == 0) {
		std::cerr << RED << "Webserv: Error: no server is defined." << RESET << std::endl;
		return false;
	}
	return true;
}

// int main(int argc, char *argv[]) {
// 	Config test;

// 	test.open(argv[1]);
// 	test.load();
// 	test.close();
// }