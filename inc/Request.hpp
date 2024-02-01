#ifndef REQUEST_HPP
#define REQUEST_HPP


#include <string>
#include <map>
#include "ClientSocket.hpp"

class Request {
    public:
        enum rqphase {
            RQLINE,
            RQHEADER,
            RQBODY,
            RQFIN
        };
    private:
        std::string _method;
        std::string _path;
        std::string _httpVersion;
        std::map<std::string, std::string> _header;
        std::string _body;
        Request::rqphase _phase;
    public:
		Request();
        void setReqphase(Request::rqphase const rqphase);
        Request::rqphase getReqphase() const;
        void setMethod(std::string const &method);
        std::string const &getMethod() const;
        void setPath(std::string const &path);
        std::string const &getPath() const;
        void setHttpVersion(std::string const &httpVersion);
        std::string const &getHttpVersion() const;
        ClientSocket::csphase load(std::stringstream &buffer);
		std::string getEntireData() const;
};

#endif
