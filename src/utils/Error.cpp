#include "Error.hpp"

GenericException::GenericException(const std::string &message)
	: errorMessage(message) {
}

GenericException::~GenericException() throw() {
}

const char *GenericException::what() const throw() {
	return (errorMessage.c_str());
}
