#ifndef UTILS_HPP
#define UTILS_HPP

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
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
bool findLF(std::stringstream &stream);
bool findCRLF(std::stringstream &stream);
bool shouldIgnoreLine(std::string const &line);
std::string rmCR(std::string const &line);
bool isNumber(std::string const &str);
Result<bool, std::string> isDirectory(std::string const &path);
bool isAccess(std::string const &path, int mode);
std::string getRandomStr(const std::size_t len);
std::string replaceUri(std::string const &uri, std::string const &a,
					   std::string const &b);
int x_close(int fd);
int x_pipe(int pfd[2]);
int x_chdir(const char *dir);
int x_dup2(int fd, int fd2);
int x_kill(int pid, int sig);
std::string trim(std::string const &line, std::string const &str);
ssize_t compCaseInsensitive(const std::string &s1, const std::string &s2);

}  // namespace utils

#endif