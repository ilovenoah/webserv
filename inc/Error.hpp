#ifndef ERROR_HPP
#define ERROR_HPP

#include <iostream>
#include <stdexcept>
#include <string>

#define ARGS_ERROR = "Too many arguments provided.\nUsage: ./webserv [config_file] or ./webserv";
#define FAIL_OPEN  = "Failed to open file.";
#define FAIL_KEY   = "Unknown key: ";

class GenericException : public std::exception
{
  private:
	std::string errorMessage;

  public:
	GenericException(const std::string &message);
	~GenericException() throw();
	const char *what() const throw();
};

#endif
