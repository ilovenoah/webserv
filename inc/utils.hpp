#ifndef UTILS_HPP
#define UTILS_HPP

#include <sys/errno.h>

#include <iostream>

#include "env.hpp"

namespace utils {
void putSysError(char const *msg);
std::size_t decStrToSizeT(std::string &str);
std::size_t hexStrToSizeT(std::string &str);
std::string sizeTtoString(std::size_t num);
bool findCRLF(std::stringstream &stream);
bool shouldIgnoreLine(std::string const &line);
bool rmCR(std::string &line);
bool isNumber(std::string const &str);
}  // namespace utils

#endif