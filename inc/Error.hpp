#ifndef ERROR_HPP
#define ERROR_HPP

#include "env.hpp"

class GenericException : public std::exception {
   private:
	std::string errorMessage;

   public:
	GenericException(const std::string &message);
	~GenericException() throw();

	const char *what() const throw();
};

#endif
