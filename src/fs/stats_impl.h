// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_SRC_FS_STATS_IMPL_H_
#define LIBNODE_SRC_FS_STATS_IMPL_H_

#include "libnode/fs/stats.h"

namespace libj {
namespace node {
namespace fs {

class StatsImpl : public Stats {
 public:
    static Ptr create() {
        return Ptr(new StatsImpl());
    }

 private:
    JsObject::Ptr obj_;

    StatsImpl() : obj_(JsObject::create()) {}

    LIBJ_JS_OBJECT_IMPL(obj_);
};

}  // namespace events
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_SRC_FS_STATS_IMPL_H_
