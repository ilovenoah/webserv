#include "Config.hpp"

std::size_t Config::lineCount = 0;

std::map<std::string, bool (Server::*)(std::string const&, std::fstream&)> Config::initSetterMap() {
	std::map<std::string, bool (Server::*)(const std::string&, std::fstream&)> srvSetterMap;
	srvSetterMap["server_name"] = &Server::setServername;
	srvSetterMap["listen"] = &Server::setListen;
	srvSetterMap["root"] = &Server::setRoot;
	srvSetterMap["allow_methods"] = &Server::setAllowMethods;
	srvSetterMap["autoindex"] = &Server::setAutoIndex;
	srvSetterMap["index"] = &Server::setIndex;
	srvSetterMap["client_body_limit"] = &Server::setClientMaxBodySize;
	srvSetterMap["cgi_extensions"] = &Server::setCgiExtensions;
	srvSetterMap["return"] = &Server::setReturn;
	srvSetterMap["error_page"] = &Server::setErrorPages;
	srvSetterMap["location"] = &Server::setLocations;
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
	this->_file.clear();
	this->_file.close();
	if (this->_file.fail()) {
		utils::putSysError("close");
		return false;
	}
	return true;
}

Server Config::_createServerInstance(std::fstream &file) {
	std::string line;
	Server server;
	bool bracketFlag = false;

	while (std::getline(file, line)) {
		Config::lineCount++;
		if (utils::shouldIgnoreLine(line)) { continue; }
		if (utils::rmCR(line) == false) { throw std::runtime_error(SYNTAX_ERROR); }
		std::stringstream ss(line);
		std::string elem;
		ss >> elem;
		if (elem.compare("}") == 0) {  bracketFlag = true; break; }
		std::map<std::string, bool (Server::*)(std::string const&, std::fstream&)>::iterator iter = this->_setterMap.find(elem);
		if (iter == this->_setterMap.end()) { throw std::runtime_error(INVALID_ITEM); }
		if (iter->first.compare("location") != 0 && line[line.size() - 1] != ';') {
			throw std::runtime_error(SYNTAX_ERROR); 
		}
		if (iter->first.compare("location") != 0 && line.size() != 0) { line = line.substr(0, line.size() - 1); }
		if ((server.*(iter->second))(line, file) == false) { throw std::runtime_error(SYNTAX_ERROR); }
	}
	if (bracketFlag == false) { throw std::runtime_error(SYNTAX_ERROR); }
	return server;
}

bool Config::load() {
	std::string line;
	try {
		while (std::getline(this->_file, line)) {
			Config::lineCount++;
			if (utils::shouldIgnoreLine(line)) { continue; }
			if (utils::rmCR(line) == false) { throw std::runtime_error(SYNTAX_ERROR); }
			std::stringstream ss(line);
			std::string elem;
			ss >> elem;
			char bracket('\0');
			ss >> bracket;
			ss >> std::ws;
			if (bracket != '{' || ss.peek() != EOF) {
				throw std::runtime_error(SYNTAX_ERROR);	
			}
			if (elem.compare("server") == 0 && bracket == '{') {
				Server server = this->_createServerInstance(this->_file);
				if (this->_servers.count(server.getServername()) > 0) {
					throw std::runtime_error(DUPULICATE_SERVER);
				}
				this->_servers.insert(std::pair<std::string, Server>(server.getServername(), server));
			}
		}
		if (this->_servers.size() == 0) {
			throw std::runtime_error(NO_SERVER);
		}
	} catch (std::exception &e) {
		std::cerr << RED << "Webserv: " << "Line " << Config::lineCount << ": Error: " << e.what() << RESET << std::endl;
		return false;
	}
	return true;
}

void Config::printServers() const {
	for (std::map<std::string, Server>::const_iterator iter = this->_servers.begin(); iter != this->_servers.end(); ++iter) {
		std::clog << "====================================" << std::endl;
		std::clog << "Server name: " << iter->second.getServername() << std::endl;
		std::clog << "Listen: " << iter->second.getListen() << std::endl;
		std::clog << "Root: " << iter->second.getRoot() << std::endl;
		std::clog << "Allow methods: ";
 		for (std::vector<std::string>::const_iterator iter2 = iter->second.getAllowMethods().begin(); iter2 != iter->second.getAllowMethods().end(); ++iter2) {
			std::clog << *iter2 << " ";
		}
		std::clog << std::endl;	
		std::clog << "Index: ";
		for (std::vector<std::string>::const_iterator iter2 = iter->second.getIndex().begin(); iter2 != iter->second.getIndex().end(); ++iter2) {
			std::clog << *iter2 << " ";
		}
		std::clog << std::endl;
		std::clog << "Autoindex: " << iter->second.getAutoindex() << std::endl;
		std::clog << "Client max body size: " << iter->second.getClientMaxBodySize() << std::endl;
		std::clog << "Cgi extensions: ";
		for (std::vector<std::string>::const_iterator iter2 = iter->second.getCgiExtensions().begin(); iter2 != iter->second.getCgiExtensions().end(); ++iter2) {
			std::clog << *iter2 << " ";
		}
		std::clog << std::endl;
		std::clog << "Return: " << iter->second.getReturn() << std::endl;
		for (std::map<std::string, std::string>::const_iterator iter2 = iter->second.getErrorPages().begin(); iter2 != iter->second.getErrorPages().end(); ++iter2) {
			std::clog << "Error page: [" << iter2->first << "] " << iter2->second << std::endl;
		}
		for (std::map<std::string, Location>::const_iterator iter2 = iter->second.getLocations().begin(); iter2 != iter->second.getLocations().end(); ++iter2) {
			std::clog << "Location: " << iter2->second.getLocationPath() << std::endl;
			std::clog << "	Allow methods: ";
			for (std::vector<std::string>::const_iterator iter3 = iter2->second.getAllowMethods().begin(); iter3 != iter2->second.getAllowMethods().end(); ++iter3) {
				std::clog << *iter3 << " ";
			}
			std::clog << std::endl;
			std::clog << "	Index: ";
			for (std::vector<std::string>::const_iterator iter3 = iter2->second.getIndex().begin(); iter3 != iter2->second.getIndex().end(); ++iter3) {
				std::clog << *iter3 << " ";
			}
			std::clog << std::endl;
			std::clog << "	Autoindex: " << iter2->second.getAutoindex() << std::endl;
			std::clog << "	Client max body size: " << iter2->second.getClientMaxBodySize() << std::endl;
			std::clog << "	Cgi extensions: ";
			for (std::vector<std::string>::const_iterator iter3 = iter2->second.getCgiExtensions().begin(); iter3 != iter2->second.getCgiExtensions().end(); ++iter3) {
				std::clog << *iter3 << " ";
			}
			std::clog << std::endl;
			std::clog << "	Return: " << iter2->second.getReturn() << std::endl;
			for (std::map<std::string, std::string>::const_iterator iter3 = iter2->second.getErrorPages().begin(); iter3 != iter2->second.getErrorPages().end(); ++iter3) {
				std::clog << "	Error page: [" << iter3->first << "] " << iter3->second << std::endl;
			}
		}
		std::clog << "====================================" << std::endl;
	}
}

// int main(int argc, char *argv[]) {
// 	Config test;

// 	test.open(argv[1]);
// 	test.load();
// 	test.close();
// }