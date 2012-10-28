// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_SRC_FLAG_H_
#define LIBNODE_SRC_FLAG_H_

#define LIBNODE_FLAG_METHODS(T, V) \
public: \
    void setFlag(T flag) { \
        V |= flag; \
    } \
    void unsetFlag(T flag) { \
        V &= ~flag; \
    } \
    Boolean hasFlag(T flag) const { \
        return V & flag; \
    }

#define LIBNODE_FLAG_IMPL(T, V) \
public: \
    void setFlag(T::Flag flag) { \
        V->setFlag(flag); \
    } \
    void unsetFlag(T::Flag flag) { \
        V->unsetFlag(flag); \
    } \
    Boolean hasFlag(T::Flag flag) const { \
        return V->hasFlag(flag); \
    }

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
