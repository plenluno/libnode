// Copyright (c) 2012 Plenluno All rights reserved.

#include "./gtest_http_common.h"

namespace libj {
namespace node {

UInt GTestHttpOnClose::count_ = 0;

JsArray::Ptr GTestHttpClientOnEnd::msgs_ = JsArray::create();

}  // namespace node
}  // namespace libj
