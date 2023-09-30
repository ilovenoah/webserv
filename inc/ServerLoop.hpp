#ifndef SERVERLOOP_HPP
#define SERVERLOOP_HPP

#include "Config.hpp"
#include "CreateSocket.hpp"
#include "env.hpp"

#define MAXEVENTS 10
#define BUFFER_SIZE 1024

int startServerLoop(Config &conf_info);

#endif
