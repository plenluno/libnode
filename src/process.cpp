// Copyright (c) 2012 Plenluno All rights reserved.

#include "libnode/process.h"
#include "libnode/timer.h"

namespace libj {
namespace node {
namespace process {

void nextTick(JsFunction::Ptr callback) {
    // TODO(plenluno): make it more efficient
    setTimeout(callback, 0);
}

}  // namespace process
}  // namespace node
}  // namespace libj
