// Copyright (c) 2013 Plenluno All rights reserved.

#ifndef LIBNODE_DEBUG_PRINT_H_
#define LIBNODE_DEBUG_PRINT_H_

#include <libnode/config.h>

#ifdef LIBNODE_DEBUG
# define LIBNODE_DEBUG_PRINT(FMT, ...) \
    libj::console::printf( \
        libj::console::LEVEL_DEBUG, \
        "[NODE DEBUG] " FMT "\n", \
        ##__VA_ARGS__);
#else
# define LIBNODE_DEBUG_PRINT(FMT, ...)
#endif

#endif  // LIBNODE_DEBUG_PRINT_H_
