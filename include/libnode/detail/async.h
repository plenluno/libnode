// Copyright (c) 2013 Plenluno All rights reserved.

#ifndef LIBNODE_DETAIL_ASYNC_H_
#define LIBNODE_DETAIL_ASYNC_H_

#include <libnode/message_queue.h>

#include <libj/exception.h>
#include <libj/executors.h>
#include <libj/detail/js_object.h>

namespace libj {
namespace node {
namespace detail {

template<typename I>
class Async : public libj::detail::JsObject<I> {
 public:
    Async(
        Size numThreads,
        ThreadFactory::Ptr threadFactory)
        : nextId_(0)
        , callbacks_(libj::Map::create())
        , msgQueue_(MessageQueue::create())
        , exec_(executors::createFixedThreadPool(
            numThreads,
            threadFactory)) {
        msgQueue_->on(
            MessageQueue::EVENT_MESSAGE,
            JsFunction::Ptr(new OnMessage(callbacks_)));
        msgQueue_->open();
    }

    virtual ~Async() {
        close();
    }

    virtual Boolean close() {
        if (!exec_) return false;

        exec_ = ExecutorService::null();  // shutdown & terminate
        msgQueue_->close();
        return true;
    }

    virtual Boolean exec(JsFunction::Ptr task, JsFunction::Ptr callback) {
        if (!exec_ || !task) return false;

        if (callback) {
            callbacks_->put(nextId_, callback);
        }
        exec_->execute(JsFunction::Ptr(new Task(nextId_, task, msgQueue_)));
        nextId_++;
        return true;
    }

 private:
    class Task : LIBJ_JS_FUNCTION_TEMPLATE(Task)
     public:
        Task(
            ULong id,
            JsFunction::Ptr task,
            MessageQueue::Ptr msgQueue)
            : id_(id)
            , task_(task)
            , msgQueue_(&(*msgQueue)) {}

        virtual Value operator()(JsArray::Ptr args) {
            Value res;
#ifdef LIBJ_USE_EXCEPTION
            try {
                res = (*task_)();
            } catch(const libj::Exception& e) {
                res = Error::create(
                    static_cast<Error::Code>(e.code()),
                    e.message());
            }
#else
            res = (*task_)();
#endif

            JsArray::Ptr msg = JsArray::create();
            msg->add(id_);
            msg->add(res);
            msgQueue_->postMessage(msg);
            return Status::OK;
        }

     private:
        ULong id_;
        JsFunction::Ptr task_;
        MessageQueue* msgQueue_;
    };

    class OnMessage : LIBJ_JS_FUNCTION_TEMPLATE(OnMessage)
     public:
        OnMessage(libj::Map::Ptr callbacks) : callbacks_(callbacks) {}

        virtual Value operator()(JsArray::Ptr args) {
            JsArray::Ptr msg = args->getPtr<JsArray>(0);
            assert(msg);
            JsFunction::Ptr callback =
                toPtr<JsFunction>(callbacks_->remove(msg->get(0)));
            if (callback) {
                callback->call(msg->get(1));
            }
            return Status::OK;
        }

     private:
        libj::Map::Ptr callbacks_;
    };

 private:
    ULong nextId_;
    libj::Map::Ptr callbacks_;
    MessageQueue::Ptr msgQueue_;
    ExecutorService::Ptr exec_;
};

}  // namespace detail
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_DETAIL_ASYNC_H_
