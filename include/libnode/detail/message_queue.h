// Copyright (c) 2013 Plenluno All rights reserved.

#ifndef LIBNODE_DETAIL_MESSAGE_QUEUE_H_
#define LIBNODE_DETAIL_MESSAGE_QUEUE_H_

#include <libnode/detail/events/event_emitter.h>

#include <libj/concurrent_linked_queue.h>

#include <uv.h>

namespace libj {
namespace node {
namespace detail {

template<typename I>
class MessageQueue : public events::EventEmitter<I> {
 public:
    MessageQueue()
        : alive_(false)
        , queue_(ConcurrentLinkedQueue::create()) {
        async_.data = this;
    }

    virtual ~MessageQueue() {
        stop();
    }

    virtual Boolean start() {
        if (alive_) {
            return false;
        } else {
            alive_ = true;
            return !uv_async_init(uv_default_loop(), &async_, receive);
        }
    }

    virtual Boolean stop() {
        if (alive_) {
            uv_close(reinterpret_cast<uv_handle_t*>(&async_), NULL);
            alive_ = false;
            return false;
        } else {
            return false;
        }
    }

    virtual Boolean postMessage(const Value& v) {
        return queue_->offer(v) && !uv_async_send(&async_);
    }

 private:
    static void receive(uv_async_t* handle, int status) {
        MessageQueue* mq = static_cast<MessageQueue*>(handle->data);
        while (!mq->queue_->isEmpty()) {
            Value msg = mq->queue_->poll();
            mq->emit(I::EVENT_MESSAGE, msg);
        }
    }

    uv_async_t async_;
    Boolean alive_;
    ConcurrentLinkedQueue::Ptr queue_;
};

}  // namespace detail
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_DETAIL_MESSAGE_QUEUE_H_
