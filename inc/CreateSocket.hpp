#ifndef WEBSERB_HPP
# define WEBSERB_HPP

# include <iostream>
# include <string>
# include <vector>
# include <unistd.h>
# include <cstring>
# include <sys/socket.h>
# include <sys/types.h>
# include <netdb.h>
# include <fcntl.h>
# include <algorithm>
# include "Config.hpp"

std::vector<int> getSockets(const std::vector<Server> &servervec);
int setNonBlocking(int fd);

#endif