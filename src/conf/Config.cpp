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

// int main(int argc, char *argv[]) {
// 	Config test;

// 	test.open(argv[1]);
// }