// Copyright (c) 2012 Plenluno All rights reserved.

#include <fcntl.h>
#include <libj/js_array.h>
#include <uv.h>
#include <string>

#include "libnode/file_system.h"

namespace libj {
namespace node {
namespace fs {

struct FileReadContext {
    uv_file file;
    std::string path;
    static const int kLen = 4096;
    char buf[kLen];
    std::string res;
    Function::Ptr cb;
};

static void onFileReadError(uv_fs_t* req) {
    FileReadContext* context = static_cast<FileReadContext*>(req->data);
    if (context->cb) {
        JsArray::Ptr args = JsArray::create();
        args->add(req->errorno);
        (*(context->cb))(args);
    }
    delete context;
    delete req;
}

static void readFileData(uv_fs_t* req, FileReadContext* context);

static void onFileRead(uv_fs_t* req) {
    if (req->errorno) {
        onFileReadError(req);
    } else if (req->result) {
        FileReadContext* context = static_cast<FileReadContext*>(req->data);
        context->res.append(std::string(context->buf, context->kLen));
        readFileData(req, context);
    } else {
        FileReadContext* context = static_cast<FileReadContext*>(req->data);
        if (context->cb) {
            JsArray::Ptr args = JsArray::create();
            args->add(req->errorno);
            args->add(String::create(context->res.c_str()));
            (*(context->cb))(args);
        }
        delete context;
        delete req;
    }
}

static void readFileData(uv_fs_t* req, FileReadContext* context) {
    uv_fs_req_cleanup(req);
    req->data = context;
    int err = uv_fs_read(
        uv_default_loop(),
        req,
        context->file,
        context->buf,
        context->kLen,
        context->res.length(),
        onFileRead);
    if (err) {
        req->errorno = err;
        onFileReadError(req);
    }
}

static void readFileAfterOpen(uv_fs_t* req) {
    if (req->errorno) {
        onFileReadError(req);
    } else {
        FileReadContext* context = static_cast<FileReadContext*>(req->data);
        context->file = req->result;
        readFileData(req, context);
    }
}

void readFile(
    String::CPtr fileName,
    JsFunction::Ptr callback) {
    uv_fs_t* req = new uv_fs_t;
    FileReadContext* context = new FileReadContext;
    context->cb = callback;
    req->data = context;
    for (Size i = 0; i < fileName->length(); i++)
        context->path += static_cast<char>(fileName->charAt(i));
    int err = uv_fs_open(
        uv_default_loop(),
        req,
        context->path.c_str(),
        O_RDONLY,
        444,
        readFileAfterOpen);
    if (err) {
        req->errorno = err;
        onFileReadError(req);
    }
}

}  // namespace fs
}  // namespace node
}  // namespace libj
