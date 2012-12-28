// Copyright (c) 2012 Plenluno All rights reserved.

#include <libnode/detail/http/agent.h>

namespace libj {
namespace node {
namespace http {

Agent::Ptr Agent::create(JsObject::CPtr options) {
    return detail::http::Agent::create(options);
}

}  // namespace http
}  // namespace node
}  // namespace libj
