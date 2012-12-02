// Copyright (c) 2012 Plenluno All rights reserved.

#include <libnode/stream.h>

namespace libj {
namespace node {

LIBJ_SYMBOL_DEF(Stream::EVENT_ERROR,         "error");
LIBJ_SYMBOL_DEF(Stream::EVENT_CLOSE,         "close");
LIBJ_SYMBOL_DEF(ReadableStream::EVENT_DATA,  "data");
LIBJ_SYMBOL_DEF(ReadableStream::EVENT_END,   "end");
LIBJ_SYMBOL_DEF(WritableStream::EVENT_DRAIN, "drain");
LIBJ_SYMBOL_DEF(WritableStream::EVENT_PIPE,  "pipe");
LIBJ_SYMBOL_DEF(DuplexStream::EVENT_DATA,    "data");
LIBJ_SYMBOL_DEF(DuplexStream::EVENT_END,     "end");
LIBJ_SYMBOL_DEF(DuplexStream::EVENT_DRAIN,   "drain");
LIBJ_SYMBOL_DEF(DuplexStream::EVENT_PIPE,    "pipe");

}  // namespace node
}  // namespace libj
