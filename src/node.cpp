// Copyright (c) 2012 Plenluno All rights reserved.

#include <uv.h>

#include "libnode/node.h"

namespace libj {
namespace node {

void run() {
    uv_run(uv_default_loop());
}

}  // namespace node
}  // namespace libj
