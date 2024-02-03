#include <string>
#include <map>

class Response {
	private:
		std::string _httpVersion;
		std::string _status;
		std::string _statusMsg;
		std::map<std::string, std::string> _headers;
		std::string _body;

	public:
		Response();

};
