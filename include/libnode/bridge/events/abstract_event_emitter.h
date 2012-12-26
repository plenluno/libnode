// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_BRIDGE_EVENTS_ABSTRACT_EVENT_EMITTER_H_
#define LIBNODE_BRIDGE_EVENTS_ABSTRACT_EVENT_EMITTER_H_

#include <libnode/events/event_emitter.h>
#include <libj/bridge/abstract_js_object.h>

namespace libj {
namespace node {
namespace bridge {
namespace events {

template<typename I>
class AbstractEventEmitter : public libj::bridge::AbstractJsObject<I> {
 public:
    AbstractEventEmitter(node::events::EventEmitter::Ptr ee)
        : libj::bridge::AbstractJsObject<I>(ee)
        , ee_(ee) {}

    virtual void on(
        String::CPtr event, JsFunction::Ptr listener) {
        ee_->on(event, listener);
    }

    virtual void once(
        String::CPtr event, JsFunction::Ptr listener) {
        ee_->once(event, listener);
    }

    virtual void addListener(
        String::CPtr event, JsFunction::Ptr listener) {
        ee_->addListener(event, listener);
    }

    virtual void removeListener(
        String::CPtr event, JsFunction::CPtr listener) {
        ee_->removeListener(event, listener);
    }

    virtual void removeAllListeners() {
        ee_->removeAllListeners();
    }

    virtual void removeAllListeners(String::CPtr event) {
        ee_->removeAllListeners(event);
    }

    virtual JsArray::Ptr listeners(String::CPtr event) {
        return ee_->listeners(event);
    }

    virtual void emit(
        String::CPtr event, JsArray::Ptr args = JsArray::null()) {
        ee_->emit(event, args);
    }

    void emit(
        String::CPtr event, const Value& v) {
        ee_->emit(event, v);
    }

    void emit(
        String::CPtr event,
        const Value& v1, const Value& v2) {
        ee_->emit(event, v1, v2);
    }

    void emit(
        String::CPtr event,
        const Value& v1, const Value& v2, const Value& v3) {
        ee_->emit(event, v1, v2, v3);
    }

    void emit(
        String::CPtr event,
        const Value& v1, const Value& v2, const Value& v3,
        const Value& v4) {
        ee_->emit(event, v1, v2, v3, v4);
    }

    void emit(
        String::CPtr event,
        const Value& v1, const Value& v2, const Value& v3,
        const Value& v4, const Value& v5) {
        ee_->emit(event, v1, v2, v3, v4, v5);
    }

    void emit(
        String::CPtr event,
        const Value& v1, const Value& v2, const Value& v3,
        const Value& v4, const Value& v5, const Value& v6) {
        ee_->emit(event, v1, v2, v3, v4, v5, v6);
    }

    void emit(
        String::CPtr event,
        const Value& v1, const Value& v2, const Value& v3,
        const Value& v4, const Value& v5, const Value& v6,
        const Value& v7) {
        ee_->emit(event, v1, v2, v3, v4, v5, v6, v7);
    }

    void emit(
        String::CPtr event,
        const Value& v1, const Value& v2, const Value& v3,
        const Value& v4, const Value& v5, const Value& v6,
        const Value& v7, const Value& v8) {
        ee_->emit(event, v1, v2, v3, v4, v5, v6, v7, v8);
    }

    void emit(
        String::CPtr event,
        const Value& v1, const Value& v2, const Value& v3,
        const Value& v4, const Value& v5, const Value& v6,
        const Value& v7, const Value& v8, const Value& v9) {
        ee_->emit(event, v1, v2, v3, v4, v5, v6, v7, v8, v9);
    }

 private:
    node::events::EventEmitter::Ptr ee_;
};

}  // namespace events
}  // namespace bridge
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_BRIDGE_EVENTS_ABSTRACT_EVENT_EMITTER_H_
