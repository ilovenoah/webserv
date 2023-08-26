#include <iostream>
#include <stdexcept>
#include <string>

class GenericException : public std::exception
{
private:
	int errorCode;
	std::string errorMessage;

public:
	GenericException(int code, const std::string &message);
	virtual ~GenericException() throw() {}
	int code() const;
	const char* what() const throw();
};
