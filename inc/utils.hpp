#ifndef UTILS_HPP
#define UTILS_HPP

#include <sys/errno.h>
#include <iostream>
#include "env.hpp"

namespace utils {
    void putSysError(char const *msg);
}

#endif