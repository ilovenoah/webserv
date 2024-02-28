#ifndef LOOP_HPP
#define LOOP_HPP

#define DEFAULT_SOCKET_TIMEOUT 30
#include <poll.h>

#include <map>
#include <utility>
#include <vector>

#include "CGIHandler.hpp"
#include "ClientSocket.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "ServerSocket.hpp"
#include "utils.hpp"

bool loop(std::map<int, ServerSocket> &ssmap, Config &config);

#endif