// Copyright (c) 2013 Plenluno All rights reserved.

#ifndef LIBNODE_JSONRPC_DETAIL_SERVICE_H_
#define LIBNODE_JSONRPC_DETAIL_SERVICE_H_

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
        : open_(true)
        , msgQueue_(MessageQueue::create())
        , methods_(libj::JsObject::create()) {
        msgQueue_->on(
            MessageQueue::EVENT_MESSAGE,
            JsFunction::Ptr(new OnMessage(this, methods_)));
        msgQueue_->open();
    }

    virtual ~Service() {
        close();
    }

    virtual void close() {
        methods_->clear();
        msgQueue_->close();
        open_ = false;
    }

    virtual Boolean addMethod(Method::CPtr method) {
        if (method && open_) {
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

 private:
    class NoResponse : LIBNODE_JSONRPC_RESPONSE(NoResponse)
     public:
        NoResponse() : emitted_(false) {}

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

    class Response10 : LIBNODE_JSONRPC_RESPONSE(Response10)
     public:
        Response10(
            Service* self,
            const Value& id)
            : self_(self)
            , id_(id)
            , emitted_(false) {}

        Boolean result(const Value& res) {
            if (emitted_) {
                return false;
            } else {
                self_->emitResult10(res, id_);
                emitted_ = true;
                return true;
            }
        }

        Boolean error(libj::Error::CPtr err) {
            if (emitted_) {
                return false;
            } else {
                self_->emitError10(err, id_);
                emitted_ = true;
                return true;
            }
        }

     private:
        Service* self_;
        Value id_;
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

            String::CPtr json = args->getCPtr<String>(0);
            Value val = json::parse(json);
            if (val.instanceof(Type<libj::Error>::id())) {
                return self_->emitError20(
                    Error::create(Error::PARSE_ERROR));
            }

            libj::JsObject::Ptr req = toPtr<libj::JsObject>(val);
            if (!req) {
                return self_->emitError20(
                    Error::create(Error::INVALID_REQUEST));
            }

            String::CPtr version = req->getCPtr<String>(JSONRPC);
            if (version && version->equals(sym20)) {
                return jsonrpc20(req);
            } else {
                return jsonrpc10(req);
            }
        }

     private:
        Int jsonrpc10(libj::JsObject::Ptr req) {
            static const Response::Ptr noResponse(new NoResponse());

            Value id = req->get(ID);
            String::CPtr name = req->getCPtr<String>(METHOD);
            JsArray::Ptr params = req->getPtr<JsArray>(PARAMS);
            if (id.isUndefined()) {
                return self_->emitError10(
                    Error::create(Error::INVALID_REQUEST));
            } else if (!name || !params) {
                return self_->emitError10(
                    Error::create(Error::INVALID_REQUEST), id);
            } else if (id.isNull()) {
                // notification
                Method::CPtr method = methods_->getCPtr<Method>(name);
                if (method) {
                    params->add(noResponse);
                    (*method->function())(params);
                }
                return Status::OK;
            } else {
                Method::CPtr method = methods_->getCPtr<Method>(name);
                if (!method) {
                    return self_->emitError10(
                        Error::create(Error::METHOD_NOT_FOUND), id);
                }

                if (params->length() != method->parameters()->length()) {
                    return self_->emitError10(
                        Error::create(Error::INVALID_PARAMS), id);
                }

                params->add(Response::Ptr(new Response10(self_, id)));
                (*method->function())(params);
                return Status::OK;
            }
        }

        Int jsonrpc20(libj::JsObject::Ptr req) {
            // TODO(plenluno): implement
            return Status::OK;
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

    Int emitResult10(const Value& r, const Value& id) {
        libj::JsObject::Ptr res = libj::JsObject::create();
        res->put(ID, id);
        res->put(RESULT, r);
        res->put(ERROR, Object::null());
        emit(EVENT_RESPONSE, res);
        return Status::OK;
    }

    Int emitError10(
        libj::Error::CPtr err,
        const Value& id = Object::null()) {
        assert(err);
        libj::JsObject::Ptr res = libj::JsObject::create();
        res->put(ID, id);
        res->put(ERROR, toJsObject(err));
        res->put(RESULT, Object::null());
        emit(EVENT_RESPONSE, res);
        return err->code();
    }

    Int emitError20(
        libj::Error::CPtr err,
        const Value& id = Object::null()) {
        LIBJ_STATIC_SYMBOL_DEF(sym20, "2.0");

        assert(err);
        libj::JsObject::Ptr res = libj::JsObject::create();
        res->put(ID, id);
        res->put(ERROR, toJsObject(err));
        res->put(JSONRPC, sym20);
        emit(EVENT_RESPONSE, res);
        return err->code();
    }

 private:
    Boolean open_;
    MessageQueue::Ptr msgQueue_;
    libj::JsObject::Ptr methods_;
};

}  // namespace detail
}  // namespace jsonrpc
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_JSONRPC_DETAIL_SERVICE_H_
