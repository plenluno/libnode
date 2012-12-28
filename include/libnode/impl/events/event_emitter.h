// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_IMPL_EVENTS_EVENT_EMITTER_H_
#define LIBNODE_IMPL_EVENTS_EVENT_EMITTER_H_

namespace libj {
namespace node {
namespace events {

inline void EventEmitter::emit(
    String::CPtr event,
    const Value& v1) {
    JsArray::Ptr args = JsArray::create();
    args->add(v1);
    emit(event, args);
}

inline void EventEmitter::emit(
    String::CPtr event,
    const Value& v1, const Value& v2) {
    JsArray::Ptr args = JsArray::create();
    args->add(v1);
    args->add(v2);
    emit(event, args);
}

inline void EventEmitter::emit(
    String::CPtr event,
    const Value& v1, const Value& v2, const Value& v3) {
    JsArray::Ptr args = JsArray::create();
    args->add(v1);
    args->add(v2);
    args->add(v3);
    emit(event, args);
}

inline void EventEmitter::emit(
    String::CPtr event,
    const Value& v1, const Value& v2, const Value& v3,
    const Value& v4) {
    JsArray::Ptr args = JsArray::create();
    args->add(v1);
    args->add(v2);
    args->add(v3);
    args->add(v4);
    emit(event, args);
}

inline void EventEmitter::emit(
    String::CPtr event,
    const Value& v1, const Value& v2, const Value& v3,
    const Value& v4, const Value& v5) {
    JsArray::Ptr args = JsArray::create();
    args->add(v1);
    args->add(v2);
    args->add(v3);
    args->add(v4);
    args->add(v5);
    emit(event, args);
}

inline void EventEmitter::emit(
    String::CPtr event,
    const Value& v1, const Value& v2, const Value& v3,
    const Value& v4, const Value& v5, const Value& v6) {
    JsArray::Ptr args = JsArray::create();
    args->add(v1);
    args->add(v2);
    args->add(v3);
    args->add(v4);
    args->add(v5);
    args->add(v6);
    emit(event, args);
}

inline void EventEmitter::emit(
    String::CPtr event,
    const Value& v1, const Value& v2, const Value& v3,
    const Value& v4, const Value& v5, const Value& v6,
    const Value& v7) {
    JsArray::Ptr args = JsArray::create();
    args->add(v1);
    args->add(v2);
    args->add(v3);
    args->add(v4);
    args->add(v5);
    args->add(v6);
    args->add(v7);
    emit(event, args);
}

inline void EventEmitter::emit(
    String::CPtr event,
    const Value& v1, const Value& v2, const Value& v3,
    const Value& v4, const Value& v5, const Value& v6,
    const Value& v7, const Value& v8) {
    JsArray::Ptr args = JsArray::create();
    args->add(v1);
    args->add(v2);
    args->add(v3);
    args->add(v4);
    args->add(v5);
    args->add(v6);
    args->add(v7);
    args->add(v8);
    emit(event, args);
}

inline void EventEmitter::emit(
    String::CPtr event,
    const Value& v1, const Value& v2, const Value& v3,
    const Value& v4, const Value& v5, const Value& v6,
    const Value& v7, const Value& v8, const Value& v9) {
    JsArray::Ptr args = JsArray::create();
    args->add(v1);
    args->add(v2);
    args->add(v3);
    args->add(v4);
    args->add(v5);
    args->add(v6);
    args->add(v7);
    args->add(v8);
    args->add(v9);
    emit(event, args);
}

}  // namespace events
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_IMPL_EVENTS_EVENT_EMITTER_H_
