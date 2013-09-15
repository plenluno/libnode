// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_DETAIL_FLAGS_H_
#define LIBNODE_DETAIL_FLAGS_H_

namespace libj {
namespace node {
namespace detail {

class Flags {
 public:
    void setFlag(UInt flag) {
        flags_ |= flag;
    }

    void unsetFlag(UInt flag) {
        flags_ &= ~flag;
    }

    void unsetAllFlags() {
        flags_ = 0;
    }

    Boolean hasFlag(UInt flag) const {
        return !!(flags_ & flag);
    }

 protected:
    UInt flags_;

    Flags() : flags_(0) {}
};

}  // namespace detail
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_DETAIL_FLAGS_H_
