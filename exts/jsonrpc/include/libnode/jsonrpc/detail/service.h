// Copyright (c) 2013-2014 Plenluno All rights reserved.

#ifndef LIBNODE_JSONRPC_DETAIL_SERVICE_H_
#define LIBNODE_JSONRPC_DETAIL_SERVICE_H_

#include <libnode/invoke.h>
#include <libnode/jsonrpc.h>
#include <libnode/message_queue.h>
#include <libnode/detail/events/event_emitter.h>

#include <assert.h>
#include <libj/json.h>

namespace libj {
namespace node {
namespace jsonrpc {
namespace detail {

class Service : public node::detail::events::EventEmitter<jsonrpc::Service> {
 public:
    Service()
        : running_(false)
        , msgQueue_(MessageQueue::create())
        , methods_(libj::JsObject::create()) {}

    virtual ~Service() {
        stop();
    }

    virtual Boolean start() {
        if (running_) {
            return false;
        } else {
            msgQueue_->on(
                MessageQueue::EVENT_MESSAGE,
                JsFunction::Ptr(new OnMessage(this, methods_)));
            msgQueue_->open();
            running_ = true;
            return true;
        }
    }

    virtual Boolean stop() {
        if (running_) {
            methods_->clear();
            msgQueue_->close();
            running_ = false;
            return true;
        } else {
            return false;
        }
    }

    virtual Boolean isRunning() const {
        return running_;
    }

    virtual Boolean addMethod(Method::CPtr method) {
        if (method && running_) {
            methods_->put(method->name(), method);
            return true;
        } else {
            return false;
        }
    }

    virtual Boolean postRequest(String::CPtr req) {
        return msgQueue_->postMessage(req);
    }

    virtual Boolean postRequest(libj::JsObject::CPtr req) {
        return postRequest(json::stringify(req));
    }

    virtual Boolean postRequest(String::CPtr req, JsFunction::Ptr cb) {
        if (cb) {
            JsArray::Ptr a = JsArray::create();
            a->add(req);
            a->add(cb);
            return msgQueue_->postMessage(a);
        } else {
            return msgQueue_->postMessage(req);
        }
    }

    virtual Boolean postRequest(libj::JsObject::CPtr req, JsFunction::Ptr cb) {
        return postRequest(json::stringify(req), cb);
    }

 private:
    class NotRespond : LIBNODE_JSONRPC_RESPOND(NotRespond)
     public:
        NotRespond() : emitted_(false) {}

        Boolean result(const Value& res) {
            if (emitted_) {
                return false;
            } else {
                emitted_ = true;
                return true;
            }
        }

        Boolean error(libj::Error::CPtr err) {
            if (emitted_) {
                return false;
            } else {
                emitted_ = true;
                return true;
            }
        }

     private:
        Boolean emitted_;
    };

    class Respond10 : LIBNODE_JSONRPC_RESPOND(Respond10)
     public:
        Respond10(
            Service* self,
            const Value& id,
            JsFunction::Ptr cb)
            : self_(self)
            , id_(id)
            , cb_(cb)
            , emitted_(false) {}

        Boolean result(const Value& res) {
            if (emitted_) {
                return false;
            } else {
                self_->emitResult10(res, id_, cb_);
                emitted_ = true;
                return true;
            }
        }

        Boolean error(libj::Error::CPtr err) {
            if (emitted_) {
                return false;
            } else {
                self_->emitError10(err, id_, cb_);
                emitted_ = true;
                return true;
            }
        }

     private:
        Service* self_;
        Value id_;
        JsFunction::Ptr cb_;
        Boolean emitted_;
    };

    class Respond20 : LIBNODE_JSONRPC_RESPOND(Respond20)
     public:
        Respond20(
            Service* self,
            const Value& id,
            JsFunction::Ptr cb)
            : self_(self)
            , id_(id)
            , cb_(cb)
            , emitted_(false) {}

        Boolean result(const Value& res) {
            if (emitted_) {
                return false;
            } else {
                self_->emitResult20(res, id_, cb_);
                emitted_ = true;
                return true;
            }
        }

        Boolean error(libj::Error::CPtr err) {
            if (emitted_) {
                return false;
            } else {
                self_->emitError20(err, id_, cb_);
                emitted_ = true;
                return true;
            }
        }

     private:
        Service* self_;
        Value id_;
        JsFunction::Ptr cb_;
        Boolean emitted_;
    };

    class OnMessage : LIBJ_JS_FUNCTION(OnMessage)
     public:
        OnMessage(
            Service* self,
            libj::JsObject::Ptr methods)
            : self_(self)
            , methods_(methods) {}

        virtual Value operator()(JsArray::Ptr args) {
            LIBJ_STATIC_SYMBOL_DEF(sym20, "2.0");

            String::CPtr json;
            JsFunction::Ptr cb;
            JsArray::Ptr a = args->getPtr<JsArray>(0);
            if (a) {
                json = a->getCPtr<String>(0);
                cb = a->getPtr<JsFunction>(1);
                assert(cb);
            } else {
                json = args->getCPtr<String>(0);
                cb = JsFunction::null();
            }

            Value val = json::parse(json);
            if (val.is<libj::Error>()) {
                return self_->emitError20(
                    Error::create(Error::PARSE_ERROR), cb);
            }

            libj::JsObject::Ptr req = toPtr<libj::JsObject>(val);
            if (!req) {
                return self_->emitError20(
                    Error::create(Error::INVALID_REQUEST), cb);
            }

            String::CPtr version = req->getCPtr<String>(JSONRPC);
            if (version && version->equals(sym20)) {
                return jsonrpc20(req, cb);
            } else {
                return jsonrpc10(req, cb);
            }
        }

     private:
        Int jsonrpc10(libj::JsObject::Ptr req, JsFunction::Ptr cb) {
            static const Respond::Ptr notRespond(new NotRespond());

            Value id = req->get(ID);
            String::CPtr name = req->getCPtr<String>(METHOD);
            JsArray::Ptr params = req->getPtr<JsArray>(PARAMS);
            if (id.isUndefined()) {
                return self_->emitError10(
                    Error::create(Error::INVALID_REQUEST), cb);
            } else if (!name || !params) {
                return self_->emitError10(
                    Error::create(Error::INVALID_REQUEST), id, cb);
            } else if (id.isNull()) {
                // notification
                Method::CPtr method = methods_->getCPtr<Method>(name);
                if (method &&
                    params->length() == method->parameters()->length()) {
                    params->add(notRespond);
                    (*method->function())(params);
                    if (cb) invoke(cb, Object::null());
                }
                return Status::OK;
            } else {
                Method::CPtr method = methods_->getCPtr<Method>(name);
                if (!method) {
                    return self_->emitError10(
                        Error::create(Error::METHOD_NOT_FOUND), id, cb);
                }

                if (params->length() != method->parameters()->length()) {
                    return self_->emitError10(
                        Error::create(Error::INVALID_PARAMS), id, cb);
                }

                params->add(Respond::Ptr(new Respond10(self_, id, cb)));
                (*method->function())(params);
                return Status::OK;
            }
        }

        Boolean checkId20(const Value& id) {
            return id.isNull()
                || id.isUndefined()
                || id.is<Long>()
                || id.is<Double>()
                || id.is<String>();
        }

        Boolean checkParams20(const Value& params) {
            return params.isUndefined()
                || params.is<JsArray>()
                || params.is<libj::JsObject>();
        }

        JsArray::Ptr toArguments20(Method::CPtr method, const Value& params) {
            JsArray::CPtr ps = method->parameters();
            Size arity = ps->length();
            if (params.isUndefined()) {
                if (!arity) {
                    return JsArray::create();
                } else {
                    return JsArray::null();
                }
            } else if (params.is<JsArray>()) {
                JsArray::Ptr args = toPtr<JsArray>(params);
                if (args->length() == arity) {
                    return args;
                } else {
                    return JsArray::null();
                }
            } else {
                assert(params.is<libj::JsObject>());
                libj::JsObject::Ptr obj = toPtr<libj::JsObject>(params);
                if (obj->size() == arity) {
                    JsArray::Ptr args = JsArray::create(arity);
                    typedef libj::Map::Entry Entry;
                    TypedSet<Entry::CPtr>::CPtr es = obj->entrySet();
                    TypedIterator<Entry::CPtr>::Ptr itr = es->iteratorTyped();
                    while (itr->hasNext()) {
                        Entry::CPtr e = itr->nextTyped();
                        Int index = ps->indexOf(e->getKey());
                        if (index >= 0) {
                            args->set(index, e->getValue());
                        } else {
                            return JsArray::null();
                        }
                    }
                    return args;
                } else {
                    return JsArray::null();
                }
            }
        }

        Int jsonrpc20(libj::JsObject::Ptr req, JsFunction::Ptr cb) {
            static const Respond::Ptr notRespond(new NotRespond());

            Value id = req->get(ID);
            Value params = req->get(PARAMS);
            String::CPtr name = req->getCPtr<String>(METHOD);
            if (!checkId20(id)) {
                return self_->emitError20(
                    Error::create(Error::INVALID_REQUEST), cb);
            } else if (!name || !checkParams20(params)) {
                return self_->emitError20(
                    Error::create(Error::INVALID_REQUEST), id, cb);
            } else if (id.isUndefined()) {
                // notification
                Method::CPtr method = methods_->getCPtr<Method>(name);
                if (!method) return Error::METHOD_NOT_FOUND;

                JsArray::Ptr args = toArguments20(method, params);
                if (!args) return Error::INVALID_PARAMS;

                args->add(notRespond);
                (*method->function())(args);
                if (cb) invoke(cb, Object::null());
                return Status::OK;
            } else {
                Method::CPtr method = methods_->getCPtr<Method>(name);
                if (!method) {
                    return self_->emitError20(
                        Error::create(Error::METHOD_NOT_FOUND), id, cb);
                }

                JsArray::Ptr args = toArguments20(method, params);
                if (!args) {
                    return self_->emitError20(
                        Error::create(Error::INVALID_PARAMS), id, cb);
                }

                args->add(Respond::Ptr(new Respond20(self_, id, cb)));
                (*method->function())(args);
                return Status::OK;
            }
        }

     private:
        Service* self_;
        libj::JsObject::Ptr methods_;
    };

 private:
    static libj::JsObject::Ptr toJsObject(libj::Error::CPtr err) {
        libj::JsObject::Ptr e = libj::JsObject::create();
        e->put(CODE, err->code());
        e->put(MESSAGE, err->message());
        return e;
    }

    Int emitResult10(
        const Value& r,
        const Value& id,
        JsFunction::Ptr cb) {
        libj::JsObject::Ptr res = libj::JsObject::create();
        res->put(ID, id);
        res->put(RESULT, r);
        res->put(ERROR, Object::null());
        emit(EVENT_RESPONSE, res);
        if (cb) invoke(cb, res);
        return Status::OK;
    }

    Int emitError10(
        libj::Error::CPtr err,
        JsFunction::Ptr cb) {
        return emitError10(err, Object::null(), cb);
    }

    Int emitError10(
        libj::Error::CPtr err,
        const Value& id,
        JsFunction::Ptr cb) {
        assert(err);
        libj::JsObject::Ptr res = libj::JsObject::create();
        res->put(ID, id);
        res->put(ERROR, toJsObject(err));
        res->put(RESULT, Object::null());
        emit(EVENT_RESPONSE, res);
        if (cb) invoke(cb, res);
        return err->code();
    }

    Int emitResult20(
        const Value& r,
        const Value& id,
        JsFunction::Ptr cb) {
        LIBJ_STATIC_SYMBOL_DEF(sym20, "2.0");

        libj::JsObject::Ptr res = libj::JsObject::create();
        res->put(ID, id);
        res->put(RESULT, r);
        res->put(JSONRPC, sym20);
        emit(EVENT_RESPONSE, res);
        if (cb) invoke(cb, res);
        return Status::OK;
    }

    Int emitError20(
        libj::Error::CPtr err,
        JsFunction::Ptr cb) {
        return emitError20(err, Object::null(), cb);
    }

    Int emitError20(
        libj::Error::CPtr err,
        const Value& id,
        JsFunction::Ptr cb) {
        LIBJ_STATIC_SYMBOL_DEF(sym20, "2.0");

        assert(err);
        libj::JsObject::Ptr res = libj::JsObject::create();
        if (id.isUndefined()) {
            res->put(ID, Object::null());
        } else {
            res->put(ID, id);
        }
        res->put(ERROR, toJsObject(err));
        res->put(JSONRPC, sym20);
        emit(EVENT_RESPONSE, res);
        if (cb) invoke(cb, res);
        return err->code();
    }

 private:
    Boolean running_;
    MessageQueue::Ptr msgQueue_;
    libj::JsObject::Ptr methods_;
};

}  // namespace detail
}  // namespace jsonrpc
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_JSONRPC_DETAIL_SERVICE_H_
