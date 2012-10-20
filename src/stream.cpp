// Copyright (c) 2012 Plenluno All rights reserved.

#include "libnode/stream.h"

namespace libj {
namespace node {

const String::CPtr Stream::EVENT_ERROR = String::intern("error");
const String::CPtr Stream::EVENT_CLOSE = String::intern("close");
const String::CPtr ReadableStream::EVENT_DATA = String::intern("data");
const String::CPtr ReadableStream::EVENT_END = String::intern("end");
const String::CPtr WritableStream::EVENT_DRAIN = String::intern("drain");
const String::CPtr WritableStream::EVENT_PIPE = String::intern("pipe");
const String::CPtr DuplexStream::EVENT_DATA = String::intern("data");
const String::CPtr DuplexStream::EVENT_END = String::intern("end");
const String::CPtr DuplexStream::EVENT_DRAIN = String::intern("drain");
const String::CPtr DuplexStream::EVENT_PIPE = String::intern("pipe");

}  // namespace node
}  // namespace libj
