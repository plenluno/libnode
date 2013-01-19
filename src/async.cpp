// Copyright (c) 2013 Plenluno All rights reserved.

#include <libnode/async.h>
#include <libnode/detail/async.h>

namespace libj {
namespace node {

Async::Ptr Async::create(
    Size numThreads,
    ThreadFactory::Ptr threadFactory) {
    return Ptr(new detail::Async<Async>(numThreads, threadFactory));
}

}  // namespace node
}  // namespace libj
