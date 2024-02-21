#include "utils.hpp"

namespace utils {
void putSysError(char const *msg) {
	std::cerr << RED << "Webserv: Error: " << msg << ": "
			  << std::strerror(errno) << RESET << std::endl;
}

std::size_t decStrToSizeT(std::string &str) {
	std::stringstream ss(str);
	std::size_t dig;

	ss >> dig;
	return dig;
}

std::size_t hexStrToSizeT(std::string &str) {
	std::stringstream ss;
	std::size_t dig;

	ss << std::hex << str;
	ss >> dig;
	return dig;
}

std::string sizeTtoString(std::size_t num) {
	std::stringstream ss;
	std::string str;

	ss << num;
	ss >> str;
	return str;
}

bool findCRLF(std::stringstream &stream) {
	std::streampos originalPos = stream.tellg();
	std::string remainingContent((std::istreambuf_iterator<char>(stream)),
								 std::istreambuf_iterator<char>());
	bool foundCRLF = remainingContent.find("\r\n") != std::string::npos;
	stream.seekg(originalPos);
	return foundCRLF;
}

bool shouldIgnoreLine(std::string const &line) {
	if (line.empty() == true) {
		return true;
	}
	std::stringstream ss(line);
	std::string elem;

	ss >> elem;
	if (elem.empty() == true) {
		return true;
	}
	if (elem[0] == '#') {
		return true;
	}
	return false;
}

std::string rmCR(std::string const &line) {
	std::string removedlline(line);
	if (removedlline.empty() == true) {
		return removedlline;
	}
	if (removedlline[removedlline.size() - 1] == '\r') {
		removedlline = removedlline.substr(0, removedlline.size() - 1);
		return removedlline;
	}
	return removedlline;
}

bool isNumber(std::string const &str) {
	for (std::string::const_iterator iter = str.begin(); iter != str.end();
		 ++iter) {
		if (std::isdigit(*iter) == false) {
			return false;
		}
	}
	return true;
}

Result<bool, std::string> isDirectory(std::string const &path) {
	struct stat statbuf;

	if (stat(path.c_str(), &statbuf) != 0) {
		utils::putSysError("stat");
		return Error<std::string>(std::strerror(errno));
	}
	if (S_ISDIR(statbuf.st_mode) == true) {
		return Ok<bool>(true);
	}
	return Ok<bool>(false);
}

bool isAccess(std::string const &path, int mode) {
	if (access(path.c_str(), mode) == -1) {
		return false;
	}
	return true;
}

std::string getRandomStr(const std::size_t len) {
	std::srand(std::time(0));
	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";
	std::string randomStr;
	randomStr.reserve(len);
	for (std::size_t i = 0; i < len; ++i) {
		randomStr += alphanum[std::rand() % (sizeof(alphanum) - 1)];
	}
	return randomStr;
}
}  // namespace utils