#include "Request.hpp"

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
    ss >> this->_method;
    ss >> this->_path;
    ss >> this->_httpVersion;
    return ClientSocket::CLOSE;
}