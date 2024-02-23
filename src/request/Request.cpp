#include "Request.hpp"

Request::Request(const std::string &remoteAddr) : _phase(Request::RQLINE), _chunksize(0), _remoteAddr(remoteAddr) {}

void Request::init() {
	this->_method.clear();
	this->_path.clear();
	this->_httpVersion.clear();
	this->_header.clear();
	this->_body.clear();
	this->_phase = Request::RQLINE;
}

void Request::setReqphase(Request::rqphase const rqphase) {
	this->_phase = rqphase;
}

Request::rqphase Request::getReqphase() const { return this->_phase; }

void Request::setMethod(std::string const &method) { this->_method = method; }

std::string const &Request::getMethod() const { return this->_method; }

void Request::setPath(std::string const &path) { this->_path = path; }

std::string const &Request::getPath() const { return this->_path; }

void Request::setHttpVersion(std::string const &httpVersion) {
	this->_httpVersion = httpVersion;
}

std::string const &Request::getHttpVersion() const {
	return this->_httpVersion;
}

Result<std::string, bool> Request::getHeaderValue(
	std::string const &key) const {
	std::map<std::string, std::string>::const_iterator iter =
		this->_header.find(key);
	if (iter == this->_header.end()) {
		return Error<bool>(false);
	}
	return Ok<std::string>(iter->second);
}

std::string const &Request::getBody() const { return this->_body; }

std::string const &Request::getRemoteAddr() const { return this->_remoteAddr; }

bool Request::shouldKeepAlive() const {
	std::map<std::string, std::string, CaseInsensitiveCompare>::const_iterator
		iter = this->_header.find("Connection");
	if (iter == this->_header.end()) {
		return true;
	}
	if (iter->second.compare("close") == 0) {
		return false;
	}
	if (iter->second.compare("keep-alive") == 0) {
		return true;
	}
	return false;
}

bool Request::isValidRequest() const {
	if (this->_method.empty() == true || this->_path.empty() == true ||
		this->_httpVersion.empty() == true) {
		return false;
	}
	if (this->_httpVersion.compare("HTTP/1.1") != 0) {
		return false;
	}
	if (this->_path.find("..") != std::string::npos) {
		return false;
	}
	std::map<std::string, std::string>::const_iterator fiter =
		this->_header.find("Transfer-Encoding");
	if (fiter != this->_header.end() && fiter->second.compare("chunked") != 0) {
		return false;
	}
	return true;
}

static std::string decodeURIComponentUTF8(std::string const &encoded) {
	std::ostringstream oss;
	for (size_t i = 0; i < encoded.size(); ++i) {
		if (encoded[i] == '%' && i + 2 < encoded.size()) {
			std::string hexStr = encoded.substr(i + 1, 2);
			std::istringstream hexStream(hexStr);
			int hexValue;
			hexStream >> std::hex >> hexValue;
			oss << (char)(hexValue);
			i += 2;
		} else if (encoded[i] == '+') {
			oss << ' ';
		} else {
			oss << encoded[i];
		}
	}
	return oss.str();
}

ClientSocket::csphase Request::load(std::stringstream &buffer) {
	ClientSocket::csphase nextcsphase(ClientSocket::CLOSE);
	switch (this->getReqphase()) {
		case Request::RQLINE: {
			std::string line;
			std::getline(buffer, line);
			if (line.compare("\r") == 0) {
				this->_phase = Request::RQLINE;
				nextcsphase = ClientSocket::RECV;
				break;
			}
			line = decodeURIComponentUTF8(line);
			line = utils::replaceUri(line, "//", "/");
			line = utils::rmCR(line);
			std::stringstream ss(line);
			ss >> this->_method;
			ss >> this->_path;
			ss >> this->_httpVersion;
			if (this->isValidRequest() == false) {
				this->_phase = Request::RQFIN;
				nextcsphase = ClientSocket::RECV;
				break;
			}
			this->_phase = Request::RQHEADER;
			nextcsphase = ClientSocket::RECV;
			break;
		}
		case Request::RQHEADER: {
			std::string line;
			std::getline(buffer, line);
			if (line.compare("\r") == 0) {
				this->_phase = Request::RQBODY;
				nextcsphase = ClientSocket::RECV;
				break;
			}
			utils::rmCR(line);
			std::stringstream ss(line);
			std::string key;
			std::string value;
			std::getline(ss, key, ':');
			ss >> std::ws;
			if (ss.peek() == EOF) {
				this->_method.clear();
				this->_path.clear();
				this->_httpVersion.clear();
				this->_phase = Request::RQFIN;
				nextcsphase = ClientSocket::RECV;
				break;
			}
			std::getline(ss, value);
			this->_header.insert(
				std::pair<std::string, std::string>(key, value));
			this->_phase = Request::RQHEADER;
			nextcsphase = ClientSocket::RECV;
			break;
		}
		case Request::RQBODY: {
			std::size_t contentLength(0);
			std::size_t expReadsize;
			std::size_t actReadsize;
			std::map<std::string, std::string>::iterator cliter =
				this->_header.find("Content-Length");
			std::map<std::string, std::string>::iterator teiter =
				this->_header.find("Transfer-Encoding");
			if (cliter == this->_header.end() &&
				teiter == this->_header.end()) {
				nextcsphase = ClientSocket::RECV;
				this->_phase = Request::RQFIN;
				break;
			} else if (cliter == this->_header.end() &&
					   teiter != this->_header.end()) {
				if (teiter->second.compare("chunked") != 0) {
					this->_phase = Request::RQFIN;
					nextcsphase = ClientSocket::RECV;
					break;
				}
				if (utils::findCRLF(buffer) == false) {
					this->_phase = Request::RQBODY;
					nextcsphase = ClientSocket::RECV;
					break;
				}
				if (this->_chunksize == 0) {
					std::string line;
					std::getline(buffer, line);
					if (line.compare("\r") == 0) {
						this->_phase = Request::RQBODY;
						nextcsphase = ClientSocket::RECV;
						break;
					}
					std::stringstream ss;
					ss << std::hex << line;
					ss >> this->_chunksize;
					if (this->_chunksize == 0) {
						nextcsphase = ClientSocket::RECV;
						this->_phase = Request::RQFIN;
						break;
					}
				}
				char buf[this->_chunksize];
				std::memset(buf, 0, this->_chunksize);
				actReadsize = buffer.readsome(buf, this->_chunksize);
				if (buffer.fail()) {
					utils::putSysError("readsome");
					nextcsphase = ClientSocket::RECV;
					this->_phase = Request::RQFIN;
					break;
				}
				this->_body.append(buf, actReadsize);
				std::string nl;
				this->_chunksize -= actReadsize;
				if (this->_body.size() > INT_MAX) {
					this->_phase = Request::RQFIN;
					nextcsphase = ClientSocket::RECV;
					break;
				}
				this->_phase = Request::RQBODY;
				nextcsphase = ClientSocket::RECV;
				break;
			}
			std::stringstream ss(cliter->second);
			ss >> contentLength;
			expReadsize = contentLength - this->_body.size();
			if (expReadsize == 0) {
				nextcsphase = ClientSocket::RECV;
				this->_phase = Request::RQFIN;
				break;
			}
			char buf[expReadsize];
			std::memset(buf, 0, expReadsize);
			actReadsize = buffer.readsome(buf, expReadsize);
			if (buffer.fail()) {
				utils::putSysError("readsome");
				nextcsphase = ClientSocket::RECV;
				this->_phase = Request::RQFIN;
				break;
			}
			this->_body.append(buf, actReadsize);
			if (this->_body.size() > INT_MAX) {
				this->_phase = Request::RQFIN;
				nextcsphase = ClientSocket::RECV;
				break;
			}
			if (this->_body.size() < contentLength) {
				this->_phase = Request::RQBODY;
				nextcsphase = ClientSocket::RECV;
				break;
			}
			nextcsphase = ClientSocket::RECV;
			this->_phase = Request::RQFIN;
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
	for (std::map<std::string, std::string>::const_iterator iter =
			 this->_header.begin();
		 iter != this->_header.end(); ++iter) {
		entireData.append(iter->first);
		entireData.append(": ");
		entireData.append(iter->second);
		entireData.append("\r\n");
	}
	entireData.append("\r\n");
	entireData.append(this->_body);
	return entireData;
}
