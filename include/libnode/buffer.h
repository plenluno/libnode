// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_BUFFER_H_
#define LIBNODE_BUFFER_H_

#include <libj/js_array_buffer.h>

namespace libj {
namespace node {

class Buffer : LIBJ_JS_ARRAY_BUFFER(Buffer)
};

}  // namespace node
}  // namespace libj

#endif  // LIBNODE_BUFFER_H_
