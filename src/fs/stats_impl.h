// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_SRC_FS_STATS_IMPL_H_
#define LIBNODE_SRC_FS_STATS_IMPL_H_

#include <libnode/fs/stats.h>

#include <libj/bridge/abstract_js_object.h>

namespace libj {
namespace node {
namespace fs {

typedef bridge::AbstractJsObject<Stats> StatsBase;

class StatsImpl : public StatsBase {
 public:
    static Ptr create() {
        return Ptr(new StatsImpl());
    }

 private:
    StatsImpl() : StatsBase(JsObject::create()) {}
};

}  // namespace events
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_SRC_FS_STATS_IMPL_H_
