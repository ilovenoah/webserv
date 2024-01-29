#ifndef LOOP_HPP
#define LOOP_HPP

#define POLLTIMEOUT 
#include <poll.h>
#include <map>
#include <vector>
#include <utility>
#include "ServerSocket.hpp"
#include "utils.hpp"

bool loop(std::map<int, ServerSocket> &ssmap);

#endif