#include "Config.hpp"

#include <iostream>

Config::Config() { _bracket_count = 0; }

Config::~Config() {}

Config::Config(const Config &copy) { *this = copy; }

Config &Config::operator=(const Config &copy) {
	if (this != &copy) {
		_servers = copy._servers;
	}
	return *this;
}

void Config::setServers(int argc, const char *argv[]) {
	parseConfig(argc, argv);
}

void Config::setFilePath(std::string file_path) { _filePath = file_path; }

const std::vector<Server> &Config::getServers() const { return _servers; }

const std::string &Config::getFilePath() const { return _filePath; }

void Config::removeUnwanted(std::string &line) {
	std::string toRemove = "\r\t{};";

	for (std::string::size_type i = 0; i < toRemove.length(); ++i) {
		line.erase(std::remove(line.begin(), line.end(), toRemove[i]),
				   line.end());
	}

	std::size_t first = line.find_first_not_of(" ");
	std::size_t last = line.find_last_not_of(" ");

	if (first == std::string::npos || last == std::string::npos) {
		line.clear();
	} else {
		line = line.substr(first, (last - first + 1));
	}
}

void Config::removeComments(std::string &line) {
	std::size_t pos;

	if ((pos = line.find('#')) != std::string::npos) {
		line.erase(pos);
	}
}

void Config::parseLine(std::string &line) {
	std::size_t pos;
	std::string key;
	std::string value;

	if ((pos = line.find("server")) != std::string::npos) {
		_servers.push_back(Server());
	} else {
		key = line.substr(0, line.find(' '));
		value = line.substr(line.find(' ') + 1);
		_servers.back().execSetterMap(key, value, _fileStream);
	}
}

void Config::parseFile() {
	std::string line;

	while (std::getline(_fileStream, line)) {
		if (line.find('{') != std::string::npos) {
			_bracket_count++;
		} else if (line.find('}') != std::string::npos) {
			_bracket_count--;
		}
		removeUnwanted(line);
		removeComments(line);
		if (line.empty()) {
			continue;
		}
		parseLine(line);
	}
	if (_bracket_count != 0) {
		throw GenericException(CONFIG_ERROR);
	}
}

void Config::readFile() {
	_fileStream.open(_filePath.c_str());

	try {
		if (_fileStream.fail()) {
			throw GenericException(FAIL_OPEN);
		}
		parseFile();
	} catch (GenericException &e) {
		std::cerr << e.what() << std::endl;
		exit(1);
	}
}

void Config::parseConfig(int argc, const char *argv[]) {
	std::string file_path = DEFAULT_CONF;

	try {
		if (argc > 2) {
			throw GenericException(ARGS_ERROR);
		}
		if (argv[1]) {
			file_path = std::string(argv[1]);
		}
		setFilePath(file_path);
		readFile();
	} catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
		exit(1);
	}
}
