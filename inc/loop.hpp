#ifndef LOOP_HPP
#define LOOP_HPP

#define POLLTIMEOUT 
#include <poll.h>
#include <map>
#include <vector>
#include <utility>
#include "Config.hpp"
#include "ServerSocket.hpp"
#include "ClientSocket.hpp"
#include "utils.hpp"
#include "Request.hpp"
#include "Response.hpp"

bool loop(std::map<int, ServerSocket> &ssmap, Config const &config);

#endif