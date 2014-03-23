// Copyright (c) 2014 Plenluno All rights reserved.

#ifndef LIBNODE_JSDP_IMPL_SERVICE_H_
#define LIBNODE_JSDP_IMPL_SERVICE_H_

#define LIBNODE_JSDP_SERVICE_INSTANCEOF(ID) \
    (ID == libj::Type<libj::node::jsdp::Service>::id() \
        || LIBJ_JS_OBJECT_INSTANCEOF(ID))

#endif  // LIBNODE_JSDP_IMPL_SERVICE_H_
