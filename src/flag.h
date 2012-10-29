// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_SRC_FLAG_H_
#define LIBNODE_SRC_FLAG_H_

namespace libj {

class FlagMixin {
 public:
    void setFlag(UInt flag) {
        flags_ |= flag;
    }

    void unsetFlag(UInt flag) {
        flags_ &= ~flag;
    }

    Boolean hasFlag(UInt flag) const {
        return flags_ & flag;
    }

 protected:
    UInt flags_;

    FlagMixin() : flags_(0) {}
};

}  // namespace libj

#endif  // LIBNODE_SRC_FLAG_H_
