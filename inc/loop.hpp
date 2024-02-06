#ifndef LOOP_HPP
#define LOOP_HPP

#define POLLTIMEOUT
#include <poll.h>

#include <map>
#include <utility>
#include <vector>

#include "ClientSocket.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "ServerSocket.hpp"
#include "utils.hpp"

bool loop(std::map<int, ServerSocket> &ssmap, Config const &config);

#endif