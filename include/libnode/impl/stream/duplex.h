// Copyright (c) 2014 Plenluno All rights reserved.

#ifndef LIBNODE_IMPL_STREAM_DUPLEX_H_
#define LIBNODE_IMPL_STREAM_DUPLEX_H_

#define LIBNODE_STREAM_DUPLEX_INSTANCEOF(ID) \
    (ID == libj::Type<libj::node::stream::Duplex>::id() \
        || LIBNODE_STREAM_INSTANCEOF(ID))

#endif  // LIBNODE_IMPL_STREAM_DUPLEX_H_
