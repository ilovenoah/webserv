#ifndef UTILS_HPP
#define UTILS_HPP

#include <sys/stat.h>
#include <sys/errno.h>
#include <errno.h>

#include <iostream>
#include "Ok.hpp"
#include "Error.hpp"
#include "Result.hpp"
#include <fcntl.h>
#include <unistd.h>

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
Result<bool, std::string> isDirectory(std::string const &path);
}  // namespace utils

#endif