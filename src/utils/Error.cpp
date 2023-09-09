#include "Error.hpp"

GenericException::GenericException(int code, const std::string &message) : errorCode(code), errorMessage(message)
{
}

int GenericException::code() const
{
	return (errorCode);
}

const char *GenericException::what() const throw()
{
	return (errorMessage.c_str());
}
