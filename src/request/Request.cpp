#include "Request.hpp"

Request::Request() : _phase(Request::RQLINE) {}

void Request::setReqphase(Request::rqphase const rqphase) {
    this->_phase = rqphase;
}

Request::rqphase Request::getReqphase() const {
    return this->_phase;
}

void Request::setMethod(std::string const &method) {
    this->_method = method;
}

std::string const &Request::getMethod() const {
    return this->_method;
}

void Request::setPath(std::string const &path){
    this->_path = path;
}

std::string const &Request::getPath() const{
    return this->_path;
}

void Request::setHttpVersion(std::string const &httpVersion){
    this->_httpVersion = httpVersion;
}

std::string const &Request::getHttpVersion() const{
    return this->_httpVersion;
}

ClientSocket::csphase Request::load(std::stringstream &buffer) {
	std::string line;
	std::getline(buffer, line);
	std::stringstream ss(line);
	ClientSocket::csphase nextcsphase(ClientSocket::CLOSE);
	switch (this->getReqphase()){
		case Request::RQLINE: {
			ss >> this->_method;
			ss >> this->_path;
			ss >> this->_httpVersion;
			this->_phase = Request::RQHEADER;
			nextcsphase = ClientSocket::RECV;
			break;
		}
		case Request::RQHEADER: {
			std::string key;
			std::string value;
			std::stringstream spaceremover;
			std::getline(ss, key, ':');
			std::getline(ss, value);
			spaceremover << value;
			value.clear();
			spaceremover >> value;
			this->_header.insert(std::pair<std::string, std::string>(key, value));
			this->_phase = Request::RQBODY;
			nextcsphase = ClientSocket::RECV;
			break;
		}
		case Request::RQBODY: {
			this->_phase = Request::RQFIN;
			nextcsphase = ClientSocket::SEND;
			break;
		}
		case Request::RQFIN: {
			this->_phase = Request::RQFIN;
			nextcsphase = ClientSocket::RECV;
			break;
		}
	}
    return nextcsphase;
}

std::string Request::getEntireData() const {
	std::string entireData;
	entireData.append(this->_method);
	entireData.append(" ");
	entireData.append(this->_path);
	entireData.append(" ");
	entireData.append(this->_httpVersion);
	entireData.append("\r\n");
	for (std::map<std::string, std::string>::const_iterator iter = this->_header.begin(); iter != this->_header.end(); ++iter) {
		entireData.append(iter->first);
		entireData.append(": ");
		entireData.append(iter->first);
		entireData.append("\r\n");
	}
	entireData.append("\r\n");
	entireData.append(this->_body);
	return entireData;
}