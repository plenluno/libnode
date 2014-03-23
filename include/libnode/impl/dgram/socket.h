// Copyright (c) 2014 Plenluno All rights reserved.

#ifndef LIBNODE_IMPL_DGRAM_SOCKET_H_
#define LIBNODE_IMPL_DGRAM_SOCKET_H_

#define LIBNODE_DGRAM_SOCKET_INSTANCEOF(ID) \
    (ID == libj::Type<libj::node::dgram::Socket>::id() \
        || LIBNODE_EVENT_EMITTER_INSTANCEOF(ID))

#endif  // LIBNODE_IMPL_DGRAM_SOCKET_H_
