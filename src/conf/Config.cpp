#include "Config.hpp"

std::size_t Config::lineCount = 0;

std::map<std::string, bool (Server::*)(std::string const &, std::fstream &)>
Config::initSetterMap() {
	std::map<std::string, bool (Server::*)(const std::string &, std::fstream &)>
		srvSetterMap;
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

std::map<std::string, bool (Server::*)(std::string const &, std::fstream &)>
	Config::_setterMap = initSetterMap();

bool Config::open(char const *path) {
	struct stat statbuf;

	if (stat(path, &statbuf) != 0) {
		utils::putSysError("stat");
		return false;
	}
	if (S_ISDIR(statbuf.st_mode) == true) {
		std::cerr << RED << "Webserv: Error: a file path is directory." << RESET
				  << std::endl;
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
		if (utils::shouldIgnoreLine(line)) {
			continue;
		}
		utils::rmCR(line);
		std::stringstream ss(line);
		std::string elem;
		ss >> elem;
		if (elem.compare("}") == 0) {
			bracketFlag = true;
			break;
		}
		std::map<std::string, bool (Server::*)(std::string const &,
											   std::fstream &)>::iterator iter =
			this->_setterMap.find(elem);
		if (iter == this->_setterMap.end()) {
			throw std::runtime_error(INVALID_ITEM);
		}
		if (iter->first.compare("location") != 0 &&
			line[line.size() - 1] != ';') {
			throw std::runtime_error(SYNTAX_ERROR);
		}
		if (iter->first.compare("location") != 0 && line.size() != 0) {
			line = line.substr(0, line.size() - 1);
		}
		if ((server.*(iter->second))(line, file) == false) {
			throw std::runtime_error(SYNTAX_ERROR);
		}
	}
	if (bracketFlag == false) {
		throw std::runtime_error(SYNTAX_ERROR);
	}
	server.fillLocationDirectives();
	return server;
}

bool Config::load() {
	std::string line;
	try {
		while (std::getline(this->_file, line)) {
			Config::lineCount++;
			if (utils::shouldIgnoreLine(line)) {
				continue;
			}
			utils::rmCR(line);
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
				std::map<std::string, std::map<std::string, Server> >::iterator smiter = this->_servers.find(server.getListen());
				if (smiter == this->_servers.end()) {
					std::map<std::string, Server> svmap;
					svmap.insert(std::pair<std::string, Server>(server.getServername(), server));
					this->_servers.insert(std::pair<std::string, std::map<std::string, Server> >(server.getListen(), svmap));
					this->_defaultServers.insert(std::pair<std::string, Server*>(server.getListen(), &(this->_servers[server.getListen()].begin()->second)));
				} else {
					if (smiter->second.count(server.getServername()) > 0) {
						throw std::runtime_error(DUPLICATE_SERVER);
					}
					smiter->second.insert(std::pair<std::string, Server>(server.getServername(), server));
				}
			}
		}
		if (this->_servers.size() == 0) {
			throw std::runtime_error(NO_SERVER);
		}
	} catch (std::exception &e) {
		std::cerr << RED << "Webserv: "
				  << "Line " << Config::lineCount << ": Error: " << e.what()
				  << RESET << std::endl;
		return false;
	}
	return true;
}

void Config::printServers() const {
	for (std::map<std::string, std::map<std::string, Server> >::const_iterator iter =
			 this->_servers.begin();
		 iter != this->_servers.end(); ++iter) {
		std::clog << "====================================" << std::endl;
		std::clog << "Default server name: " << this->getDefaultServer(iter->first).getOk()->getServername() << std::endl;
		for (std::map<std::string, Server>::const_iterator iter2 = iter->second.begin(); iter2 != iter->second.end(); ++iter2) {

			std::clog << "====================================" << std::endl;
			std::clog << "Server name: " << iter2->second.getServername()
					<< std::endl;
			std::clog << "Listen: " << iter2->second.getListen() << std::endl;
			std::clog << "Root: " << iter2->second.getRoot() << std::endl;
			std::clog << "Allow methods: ";
			for (std::vector<std::string>::const_iterator iter3 =
					iter2->second.getAllowMethods().begin();
				iter3 != iter2->second.getAllowMethods().end(); ++iter3) {
				std::clog << *iter3 << " ";
			}
			std::clog << std::endl;
			std::clog << "Index: ";
			for (std::vector<std::string>::const_iterator iter3 =
					iter2->second.getIndex().begin();
				iter3 != iter2->second.getIndex().end(); ++iter3) {
				std::clog << *iter3 << " ";
			}
			std::clog << std::endl;
			std::clog << "Autoindex: " << iter2->second.getAutoindex() << std::endl;
			std::clog << "Client max body size: "
					<< iter2->second.getClientMaxBodySize() << std::endl;
			std::clog << "Cgi extensions: ";
			for (std::vector<std::string>::const_iterator iter3 =
					iter2->second.getCgiExtensions().begin();
				iter3 != iter2->second.getCgiExtensions().end(); ++iter3) {
				std::clog << *iter3 << " ";
			}
			std::clog << std::endl;
			std::clog << "Return: " << iter2->second.getReturn() << std::endl;
			for (std::map<std::string, std::string>::const_iterator iter3 =
					iter2->second.getErrorPages().begin();
				iter3 != iter2->second.getErrorPages().end(); ++iter3) {
				std::clog << "Error page: [" << iter3->first << "] "
						<< iter3->second << std::endl;
			}
			for (std::map<std::string, Location>::const_iterator iter3 =
					iter2->second.getLocations().begin();
				iter3 != iter2->second.getLocations().end(); ++iter3) {
				std::clog << "Location: " << iter3->second.getLocationPath()
						<< std::endl;
				std::clog << "	Allow methods: ";
				for (std::vector<std::string>::const_iterator iter4 =
						iter3->second.getAllowMethods().begin();
					iter4 != iter3->second.getAllowMethods().end(); ++iter4) {
					std::clog << *iter4 << " ";
				}
				std::clog << std::endl;
				std::clog << "	Index: ";
				for (std::vector<std::string>::const_iterator iter4 =
						iter3->second.getIndex().begin();
					iter4 != iter3->second.getIndex().end(); ++iter4) {
					std::clog << *iter4 << " ";
				}
				std::clog << std::endl;
				std::clog << "	Autoindex: " << iter3->second.getAutoindex()
						<< std::endl;
				std::clog << "	Client max body size: "
						<< iter3->second.getClientMaxBodySize() << std::endl;
				std::clog << "	Cgi extensions: ";
				for (std::vector<std::string>::const_iterator iter4 =
						iter3->second.getCgiExtensions().begin();
					iter4 != iter3->second.getCgiExtensions().end(); ++iter4) {
					std::clog << *iter4 << " ";
				}
				std::clog << std::endl;
				std::clog << "	Return: " << iter3->second.getReturn() << std::endl;
				for (std::map<std::string, std::string>::const_iterator iter4 =
						iter3->second.getErrorPages().begin();
					iter4 != iter3->second.getErrorPages().end(); ++iter4) {
					std::clog << "	Error page: [" << iter4->first << "] "
							<< iter4->second << std::endl;
				}
			}
			std::clog << "====================================" << std::endl;
		}
	}
}

const Result<Server*, bool>  Config::getDefaultServer(const std::string &listen) const {
	std::map<std::string, Server*>::const_iterator sviter = this->_defaultServers.find(listen);
	if (sviter == this->_defaultServers.end()) { return Error<bool>(false); }
	return Ok<Server*>(sviter->second);
}

const std::map<std::string, std::map<std::string, Server> > &Config::getServers() const {
	return this->_servers;
}

Server *Config::getServerPointer(const std::string &listen, const std::string &hostName) {
	std::map<std::string, std::map<std::string, Server> >::iterator ipiter = this->_servers.find(listen);
	std::map<std::string, Server>::iterator sviter = ipiter->second.find(hostName);

	if (sviter == ipiter->second.end()) { return this->_defaultServers.find(listen)->second; }
	return &(sviter->second);
}