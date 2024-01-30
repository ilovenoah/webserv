#ifndef REQUEST_HPP
#define REQUEST_HPP


#include <string>
#include <map>

class Request {
    private:
        std::string _method;
        std::string _path;
        std::string _httpVersion;
        std::map<std::string, std::string> _header;
        std::string _body;
    public:
        void setMethod(std::string const &method);
        std::string const &getMethod() const;
        void setPath(std::string const &path);
        std::string const &getPath() const;
        void setHttpVersion(std::string const &httpVersion);
        std::string const &getHttpVersion() const;
};

#endif