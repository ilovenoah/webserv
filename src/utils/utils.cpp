#include "utils.hpp"

namespace utils {
	void putSysError(char const *msg) {
		std::cerr << RED << "Webserv: Error: " << msg << ": " << std::strerror(errno) << RESET << std::endl;
	}
}