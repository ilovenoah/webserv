#ifndef WEBSERB_HPP
#define WEBSERB_HPP

#include "Config.hpp"
#include "env.hpp"

std::vector<int> getSockets(const std::vector<Server> &servervec);
int setNonBlocking(int fd);

#endif
