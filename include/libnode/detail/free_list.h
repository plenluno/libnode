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
        , list_(TypedLinkedList<T>::create()) {}

    T alloc() {
        if (list_->isEmpty()) {
            T t;  // null
            return t;
        } else {
            return list_->shiftTyped();
        }
    }

    void free(T t) {
        if (list_->length() < max_) {
            list_->addTyped(t);
        }
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
        , list_(TypedLinkedList<T>::create()) {}

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
