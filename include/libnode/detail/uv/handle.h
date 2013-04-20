// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_DETAIL_UV_HANDLE_H_
#define LIBNODE_DETAIL_UV_HANDLE_H_

#include <libnode/uv/error.h>

#include <assert.h>

namespace libj {
namespace node {
namespace detail {
namespace uv {

class Handle : public libj::detail::GCBase {
 protected:
    Handle(uv_handle_t* handle)
        : handle_(handle)
        , unref_(false) {
        assert(handle_);
        handle_->data = this;
    }

 public:
    virtual ~Handle() {}

    virtual void setHandle(uv_handle_t* handle) {
        handle_ = handle;
        handle_->data = this;
    }

    uv_handle_type type() const { return handle_->type; }

    void ref() {
        uv_ref(handle_);
        unref_ = false;
    }

    void unref() {
        uv_unref(handle_);
        unref_ = true;
    }

    void close() {
        if (handle_) {
            uv_close(handle_, onClose);
            handle_ = NULL;
        }
    }

 public:
    static uv_handle_type guessHandleType(int fd) {
        assert(fd >= 0);
        return uv_guess_handle(fd);
    }

 protected:
    static void setLastError() {
        node::uv::Error::setLast(uv_last_error(uv_default_loop()).code);
    }

 private:
    static void onClose(uv_handle_t* handle) {
        Handle* self = static_cast<Handle*>(handle->data);
        assert(self->handle_ == NULL);
        delete self;
    }

 protected:
    uv_handle_t* handle_;
    bool unref_;
};

}  // namespace uv
}  // namespace detail
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_DETAIL_UV_HANDLE_H_
