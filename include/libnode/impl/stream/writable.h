// Copyright (c) 2014 Plenluno All rights reserved.

#ifndef LIBNODE_IMPL_STREAM_WRITABLE_H_
#define LIBNODE_IMPL_STREAM_WRITABLE_H_

#define LIBNODE_STREAM_WRITABLE_INSTANCEOF(ID) \
    (ID == libj::Type<libj::node::stream::Writable>::id() \
        || LIBNODE_STREAM_INSTANCEOF(ID))

#endif  // LIBNODE_IMPL_STREAM_WRITABLE_H_
