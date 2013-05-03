// Copyright (c) 2012-2013 Plenluno All rights reserved.

#ifndef LIBNODE_GTEST_GTEST_COMMON_H_
#define LIBNODE_GTEST_GTEST_COMMON_H_

#include <libnode/buffer.h>
#include <libnode/node.h>

#include <libj/debug_print.h>
#include <libj/js_function.h>
#include <libj/string_buffer.h>

#include <assert.h>

namespace libj {
namespace node {

class GTestOnData : LIBJ_JS_FUNCTION(GTestOnData)
 public:
    GTestOnData()
        : bufs_(JsArray::create())
        , strBuf_(StringBuffer::create()) {}

    static UInt count() { return count_; }

    static void clear() { count_ = 0; }

    virtual Value operator()(JsArray::Ptr args) {
        count_++;
        Buffer::CPtr buf = toCPtr<Buffer>(args->get(0));
        String::CPtr str = toCPtr<String>(args->get(0));
        if (buf) {
            bufs_->add(buf);
        } else if (str) {
            strBuf_->append(str);
        } else {
            assert(false);
        }
        return Status::OK;
    }

    Value data() const {
        if (bufs_->isEmpty()) {
            return string();
        } else {
            return buffer();
        }
    }

    Buffer::CPtr buffer() const { return Buffer::concat(bufs_); }

    String::CPtr string() const { return strBuf_->toString(); }

 private:
    static UInt count_;

    JsArray::Ptr bufs_;
    StringBuffer::Ptr strBuf_;
};

class GTestOnClose :  LIBJ_JS_FUNCTION(GTestOnClose)
 public:
    static UInt count() { return count_; }

    static void clear() { count_ = 0; }

    virtual Value operator()(JsArray::Ptr args) {
        count_++;
        return Status::OK;
    }

 private:
    static UInt count_;
};

class GTestOnEnd : LIBJ_JS_FUNCTION(GTestOnEnd)
 public:
    GTestOnEnd(GTestOnData::Ptr onData)
        : onData_(onData) {}

    static JsArray::Ptr messages() {
        if (!msgs_) {
            msgs_ = JsArray::create();
            LIBJ_DEBUG_PRINT(
                "static: JsArray %p",
                LIBJ_DEBUG_OBJECT_PTR(msgs_));
        }
        return msgs_;
    }

    static void clear() {
        messages()->clear();
    }

    virtual Value operator()(JsArray::Ptr args) {
        messages()->add(onData_->data());
        return Status::OK;
    }

 private:
    static JsArray::Ptr msgs_;

    GTestOnData::Ptr onData_;
};

}  // namespace node
}  // namespace libj

#endif  // LIBNODE_GTEST_GTEST_COMMON_H_
