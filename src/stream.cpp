// Copyright (c) 2012 Plenluno All rights reserved.

#include "libnode/stream.h"

namespace libj {
namespace node {

const String::CPtr Stream::EVENT_ERROR = String::create("error");
const String::CPtr Stream::EVENT_CLOSE = String::create("close");
const String::CPtr ReadableStream::EVENT_DATA = String::create("data");
const String::CPtr ReadableStream::EVENT_END = String::create("end");
const String::CPtr WritableStream::EVENT_DRAIN = String::create("drain");
const String::CPtr WritableStream::EVENT_PIPE = String::create("pipe");
const String::CPtr DuplexStream::EVENT_DATA = String::create("data");
const String::CPtr DuplexStream::EVENT_END = String::create("end");
const String::CPtr DuplexStream::EVENT_DRAIN = String::create("drain");
const String::CPtr DuplexStream::EVENT_PIPE = String::create("pipe");

}  // namespace node
}  // namespace libj
