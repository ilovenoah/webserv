#include "utils.hpp"

std::vector<std::string> split(std::string line, const char *sep) {
	std::vector<std::string> lines = std::vector<std::string>();
	std::size_t pos;

	while ((pos = line.find(sep)) != std::string::npos){
		lines.push_back(line.substr(0, pos));
		line.erase(0, pos + 1);
	}
	lines.push_back(line);
	return lines;
}
