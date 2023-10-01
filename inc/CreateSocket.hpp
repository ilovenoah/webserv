#ifndef CREATESOCKET_HPP
#define CREATESOCKET_HPP

#include "Config.hpp"
#include "env.hpp"

#define MAXLEN 10

std::vector<int> getSockets(const std::vector<Server> &servervec);
int setNonBlocking(int fd);

#endif
