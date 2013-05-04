// Copyright (c) 2012 Plenluno All rights reserved.

#include "./gtest_http_common.h"

namespace libj {
namespace node {

UInt GTestHttpServerOnEnd::count_ = 0;

JsArray::Ptr GTestHttpClientOnResponse::statusCodes_ = JsArray::null();

}  // namespace node
}  // namespace libj
