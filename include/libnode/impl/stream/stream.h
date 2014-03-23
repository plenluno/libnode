// Copyright (c) 2014 Plenluno All rights reserved.

#ifndef LIBNODE_IMPL_STREAM_STREAM_H_
#define LIBNODE_IMPL_STREAM_STREAM_H_

#define LIBNODE_STREAM_INSTANCEOF(ID) \
    (ID == libj::Type<libj::node::stream::Stream>::id() \
        || LIBNODE_EVENT_EMITTER_INSTANCEOF(ID))

#endif  // LIBNODE_IMPL_STREAM_STREAM_H_
