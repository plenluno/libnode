// Copyright (c) 2012 Plenluno All rights reserved.

#include "./gtest_common.h"

namespace libj {
namespace node {

UInt GTestOnData::count_ = 0;

UInt GTestOnClose::count_ = 0;

JsArray::Ptr GTestOnEnd::msgs_ = JsArray::create();

}  // namespace node
}  // namespace libj
