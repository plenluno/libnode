// Copyright (c) 2013 Plenluno All rights reserved.

#ifndef LIBNODE_DETAIL_MESSAGE_QUEUE_H_
#define LIBNODE_DETAIL_MESSAGE_QUEUE_H_

#include <libnode/config.h>
#include <libnode/detail/events/event_emitter.h>

#include <libj/concurrent_linked_queue.h>

#include <uv.h>
#include <assert.h>

namespace libj {
namespace node {
namespace detail {

template<typename I>
class MessageQueue : public events::EventEmitter<I> {
 public:
    MessageQueue()
        : open_(false)
        , queue_(ConcurrentLinkedQueue::create()) {
        async_.data = this;
    }

    virtual ~MessageQueue() {
        close();
    }

    virtual Boolean open() {
        assert(queue_->isEmpty());
        if (open_) {
            return false;
        } else {
            open_ = true;
            return !uv_async_init(uv_default_loop(), &async_, receive);
        }
    }

    virtual Boolean close() {
        if (open_) {
            open_ = false;
            uv_close(reinterpret_cast<uv_handle_t*>(&async_), NULL);
            emitMessage(this);
#ifdef LIBNODE_REMOVE_LISTENER
            this->removeAllListeners();
#endif
            return true;
        } else {
            return false;
        }
    }

    virtual Boolean postMessage(const Value& v) {
        if (open_) {
            return queue_->offer(v) && !uv_async_send(&async_);
        } else {
            return false;
        }
    }

 private:
    static void receive(uv_async_t* handle, int status) {
        MessageQueue* mq = static_cast<MessageQueue*>(handle->data);
        emitMessage(mq);
    }

    static void emitMessage(MessageQueue* mq) {
        while (!mq->queue_->isEmpty()) {
            Value msg = mq->queue_->poll();
            mq->emit(I::EVENT_MESSAGE, msg);
        }
    }

    uv_async_t async_;
    Boolean open_;
    ConcurrentLinkedQueue::Ptr queue_;
};

}  // namespace detail
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_DETAIL_MESSAGE_QUEUE_H_
