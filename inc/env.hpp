#ifndef ENV_HPP
#define ENV_HPP

#include <fcntl.h>
#include <netdb.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <algorithm>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#define ARGS_ERROR "Too many arguments provided.\nUsage: ./webserv [config_file] or ./webserv"
#define FAIL_OPEN "Failed to open file."
#define UNKOWN_KEY "Unknown key: "

#endif
