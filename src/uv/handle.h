// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_SRC_UV_HANDLE_H_
#define LIBNODE_SRC_UV_HANDLE_H_

#include <assert.h>
#include <uv.h>

#include "libnode/uv/error.h"

namespace libj {
namespace node {
namespace uv {

class Handle {
 public:
    uv_handle_t* uvHandle() const { return handle_; }

    virtual ~Handle() {}

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

    virtual void setHandle(uv_handle_t* handle) {
        handle_ = handle;
        handle_->data = this;
    }

 protected:
    Handle(uv_handle_t* handle)
        : handle_(handle)
        , unref_(false) {
        assert(handle_);
        handle_->data = this;
    }

    static void setLastError() {
        Error::setLast(uv_last_error(uv_default_loop()).code);
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
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_SRC_UV_HANDLE_H_
