// Copyright (c) 2014 Plenluno All rights reserved.

#ifndef LIBNODE_IMPL_FS_STATS_H_
#define LIBNODE_IMPL_FS_STATS_H_

#define LIBNODE_FS_STATS_INSTANCEOF(ID) \
    (ID == libj::Type<libj::node::fs::Stats>::id() \
        || LIBJ_JS_OBJECT_INSTANCEOF(ID))

#endif  // LIBNODE_IMPL_FS_STATS_H_
