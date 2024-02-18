#ifndef UTILS_HPP
#define UTILS_HPP

#include <errno.h>
#include <fcntl.h>
#include <sys/errno.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstdlib>
#include <ctime>
#include <iostream>

#include "Error.hpp"
#include "Ok.hpp"
#include "Result.hpp"
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
bool isAccess(std::string const &path, int mode);
std::string getRandomStr(const std::size_t len);
}  // namespace utils

#endif