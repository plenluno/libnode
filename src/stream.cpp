// Copyright (c) 2012 Plenluno All rights reserved.

#include <libnode/stream.h>

namespace libj {
namespace node {
namespace stream {

LIBJ_SYMBOL_DEF(Stream::EVENT_ERROR,   "error");
LIBJ_SYMBOL_DEF(Stream::EVENT_CLOSE,   "close");
LIBJ_SYMBOL_DEF(Readable::EVENT_DATA,  "data");
LIBJ_SYMBOL_DEF(Readable::EVENT_END,   "end");
LIBJ_SYMBOL_DEF(Writable::EVENT_DRAIN, "drain");
LIBJ_SYMBOL_DEF(Writable::EVENT_PIPE,  "pipe");
LIBJ_SYMBOL_DEF(Duplex::EVENT_DATA,    "data");
LIBJ_SYMBOL_DEF(Duplex::EVENT_END,     "end");
LIBJ_SYMBOL_DEF(Duplex::EVENT_DRAIN,   "drain");
LIBJ_SYMBOL_DEF(Duplex::EVENT_PIPE,    "pipe");

}  // namespace stream
}  // namespace node
}  // namespace libj
