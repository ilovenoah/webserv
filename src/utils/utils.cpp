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

bool rmCR(std::string &line) {
	if (line.empty() == true) {
		return false;
	}
	if (line[line.size() - 1] == '\r') {
		line = line.substr(0, line.size() - 1);
		return true;
	}
	return false;
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

	if (access(path.c_str(), F_OK) == -1) {
		utils::putSysError("access");
		return Error<std::string>(std::strerror(errno));
	}
	if (stat(path.c_str(), &statbuf) != 0) {
		utils::putSysError("stat");
		return Error<std::string>(std::strerror(errno));
	}
	if (S_ISDIR(statbuf.st_mode) == true) {
		return Ok<bool>(true);
	}
	return Ok<bool>(false);
}
}  // namespace utils