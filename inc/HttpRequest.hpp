#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include "env.hpp"

class HttpRequest {
   private:
	std::string _method;
	std::string _version;
	std::string _path;
	std::string _body;

   public:
};

#endif
