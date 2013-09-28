// Copyright (c) 2013 Plenluno All rights reserved.

#ifndef LIBNODE_DETAIL_FREE_LIST_H_
#define LIBNODE_DETAIL_FREE_LIST_H_

#include <libj/typed_linked_list.h>

namespace libj {
namespace node {
namespace detail {

template<
    typename T,
    Boolean IsObject = libj::detail::Classify<T>::isObject>
class FreeList {
 public:
    FreeList(Size max)
        : max_(max)
        , list_(TypedLinkedList<T>::create()) {
        LIBJ_DEBUG_PRINT(
            "static: FreeList %p",
            LIBJ_DEBUG_OBJECT_PTR(list_));
    }

    T alloc() {
        if (list_->isEmpty()) {
#ifdef LIBJ_USE_SP
            T t;  // null
            return t;
#else
            return NULL;
#endif
        } else {
            return list_->shiftTyped();
        }
    }

    void free(T t) {
        if (list_->length() < max_) {
            list_->addTyped(t);
        }
    }

    void clear() {
        list_->clear();
    }

    Size length() const {
        return list_->length();
    }

 private:
    Size max_;
    typename TypedLinkedList<T>::Ptr list_;
};

template<typename T>
class FreeList<T, false> {
 public:
    FreeList(Size max)
        : max_(max)
        , list_(TypedLinkedList<T>::create()) {
        LIBJ_DEBUG_PRINT(
            "static: FreeList %p",
            LIBJ_DEBUG_OBJECT_PTR(list_));
    }

    virtual ~FreeList() {
        while (!list_->isEmpty()) {
            delete alloc();
        }
    }

    T alloc() {
        if (list_->isEmpty()) {
            return NULL;
        } else {
            return list_->shiftTyped();
        }
    }

    void free(T t) {
        if (list_->length() < max_) {
            list_->addTyped(t);
        } else {
            delete t;
        }
    }

    void clear() {
        list_->clear();
    }

    Size length() const {
        return list_->length();
    }

 private:
    Size max_;
    typename TypedLinkedList<T>::Ptr list_;
};

}  // namespace detail
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_DETAIL_FREE_LIST_H_
