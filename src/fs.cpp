// Copyright (c) 2012 Plenluno All rights reserved.

#include <fcntl.h>
#include <libj/js_array.h>
#include <uv.h>
#include <string>

#include "libnode/buffer.h"
#include "libnode/fs.h"
#include "./fs/stats_impl.h"

namespace libj {
namespace node {
namespace fs {

class Context {
 public:
    uv_fs_t* req;
    uv_file file;
    std::string path;
    Function::Ptr callback;

    Context(Function::Ptr cb)
        : req(new uv_fs_t)
        , callback(cb) {
        req->errorno = 0;
        req->data = this;
    }

    virtual ~Context() {
        uv_fs_req_cleanup(req);
        delete req;
    }
};

class ReadContext : public Context {
 public:
    char* buffer;
    Size length;
    Buffer::Ptr res;
    Size offset;

    ReadContext(Size len, Function::Ptr cb)
        : Context(cb)
        , buffer(new char[len])
        , length(len)
        , res(LIBJ_NULL(Buffer))
        , offset(0) {
        req->data = this;
    }

    ~ReadContext() {
        delete [] buffer;
    }
};

static Int convertFlag(Flag flag) {
    switch (flag) {
    case R:
        return O_RDONLY;
    case RS:
        return O_RDONLY | O_SYNC;
    case RP:
        return O_RDWR;
    case RSP:
        return O_RDWR | O_SYNC;
    case W:
        return O_TRUNC | O_CREAT | O_WRONLY;
    case WX:
        return O_TRUNC | O_CREAT | O_WRONLY | O_EXCL;
    case WP:
        return O_TRUNC | O_CREAT | O_RDWR;
    case WXP:
        return O_TRUNC | O_CREAT | O_RDWR | O_EXCL;
    case A:
        return O_APPEND | O_CREAT | O_WRONLY;
    case AX:
        return O_APPEND | O_CREAT | O_WRONLY | O_EXCL;
    case AP:
        return O_APPEND | O_CREAT | O_RDWR;
    case AXP:
        return O_APPEND | O_CREAT | O_RDWR | O_EXCL;
    default:
        return -1;
    }
}

static void onError(uv_fs_t* req) {
    Context* context = static_cast<Context*>(req->data);
    if (context->callback) {
        JsArray::Ptr args = JsArray::create();
        args->add(req->errorno);
        (*(context->callback))(args);
    }
    delete context;
}

static Stats::Ptr getStats(uv_fs_t* req) {
    Stats::Ptr stats = StatsImpl::create();
    const struct stat* s = static_cast<const struct stat*>(req->ptr);
    stats->put(STAT_DEV, static_cast<Long>(s->st_dev));
    stats->put(STAT_INO, static_cast<Long>(s->st_ino));
    stats->put(STAT_MODE, static_cast<Long>(s->st_mode));
    stats->put(STAT_NLINK, static_cast<Long>(s->st_nlink));
    stats->put(STAT_UID, static_cast<Long>(s->st_uid));
    stats->put(STAT_GID, static_cast<Long>(s->st_gid));
    stats->put(STAT_RDEV, static_cast<Long>(s->st_rdev));
    stats->put(STAT_SIZE, static_cast<Long>(s->st_size));
    stats->put(STAT_BLKSIZE, static_cast<Long>(s->st_blksize));
    stats->put(STAT_BLOCKS, static_cast<Long>(s->st_blocks));
    stats->put(STAT_ATIME, static_cast<Long>(s->st_atime));
    stats->put(STAT_MTIME, static_cast<Long>(s->st_mtime));
    stats->put(STAT_CTIME, static_cast<Long>(s->st_ctime));
    return stats;
}

static Buffer::Ptr getBuffer(uv_fs_t* req) {
    ReadContext* context = static_cast<ReadContext*>(req->data);
    char* buf = context->buffer;
    Size offset = context->offset;
    Size bytesRead = req->result;
    Buffer::Ptr res = context->res;
    for (Size i = 0; i < bytesRead; i++) {
        res->writeUInt8(buf[i], offset + i);
    }
    return res;
}

static void after(uv_fs_t* req) {
    if (req->result == -1) {
        onError(req);
    } else {
        Context* context = static_cast<Context*>(req->data);
        if (context->callback) {
            JsArray::Ptr args = JsArray::create();
            args->add(0);
            switch (req->fs_type) {
            case UV_FS_OPEN:
                args->add(static_cast<uv_file>(req->result));
                break;
            case UV_FS_READ:
                args->add(static_cast<Size>(req->result));
                args->add(getBuffer(req)->toString());
                break;
            case UV_FS_STAT:
                args->add(getStats(req));
                break;
            case UV_FS_CLOSE:
            default:
                break;
            }
            (*(context->callback))(args);
        }
        delete context;
    }
}

void open(String::CPtr path, Flag flag, JsFunction::Ptr callback) {
    if (!callback)
        return;
    Context* context = new Context(callback);
    context->path = path->toStdString();
    uv_fs_t* req = context->req;
    int err = uv_fs_open(
        uv_default_loop(),
        req,
        context->path.c_str(),
        convertFlag(flag),
        438,
        after);
    if (err) {
        req->errorno = err;
        onError(req);
    }
}

void close(const Value& fd, JsFunction::Ptr callback) {
    Context* context = new Context(callback);
    uv_fs_t* req = context->req;
    if (!to<uv_file>(fd, &(context->file))) {
        req->errorno = UV_EINVAL;
        onError(req);
        return;
    }
    int err = uv_fs_close(
        uv_default_loop(),
        req,
        context->file,
        after);
    if (err) {
        req->errorno = err;
        onError(req);
    }
}

void read(
    const Value &fd, const Value& buffer, Size offset,
    Size length, Size position, JsFunction::Ptr callback) {
    Buffer::Ptr buf = toPtr<Buffer>(buffer);
    Size len = 0;
    Size bufLen = buf ? buf->length() : 0;
    if (length > 0 && offset < bufLen) {
        len = bufLen - offset;
        len = len < length ? len : length;
    }
    ReadContext* context = new ReadContext(len, callback);
    uv_fs_t* req = context->req;
    if (!len) {
        req->errorno = UV_EINVAL;
        onError(req);
        return;
    }

    if (!to<uv_file>(fd, &(context->file))) {
        req->errorno = UV_EINVAL;
        onError(req);
        return;
    }
    context->res = buf;
    context->offset = offset;

    int err = uv_fs_read(
        uv_default_loop(),
        req,
        context->file,
        context->buffer,
        len,
        position,
        after);
    if (err) {
        req->errorno = err;
        onError(req);
    }
}

void stat(String::CPtr path, JsFunction::Ptr callback) {
    if (!callback)
        return;
    Context* context = new Context(callback);
    if (path)
        context->path = path->toStdString();
    uv_fs_t* req = context->req;
    int err = uv_fs_stat(
        uv_default_loop(),
        req,
        context->path.c_str(),
        after);
    if (err) {
        req->errorno = err;
        onError(req);
    }
}

class AfterReadInReadFile : LIBJ_JS_FUNCTION(AfterReadInReadFile)
 private:
    Value fd_;
    JsFunction::Ptr callback_;

 public:
    AfterReadInReadFile(
        const Value& fd,
        JsFunction::Ptr callback)
        : fd_(fd)
        , callback_(callback) {}

    Value operator()(JsArray::Ptr args) {
        Int err = -1;
        to<Int>(args->get(0), &err);
        JsArray::Ptr a = JsArray::create();
        a->add(err);
        if (!err) {
            a->add(args->get(2));
        }
        (*callback_)(a);
        LIBJ_NULL_PTR(JsFunction, nullp);
        close(fd_, nullp);
        return 0;
    }
};

class AfterOpenInReadFile : LIBJ_JS_FUNCTION(AfterOpenInReadFile)
 private:
    Value res_;
    Size length_;
    JsFunction::Ptr callback_;

 public:
    AfterOpenInReadFile(
        const Value& res,
        Size len,
        JsFunction::Ptr callback)
        : res_(res)
        , length_(len)
        , callback_(callback) {}

    Value operator()(JsArray::Ptr args) {
        Int err = -1;
        to<Int>(args->get(0), &err);
        if (err) {
            JsArray::Ptr a = JsArray::create();
            a->add(err);
            (*callback_)(a);
        } else {
            Value fd = args->get(1);
            AfterReadInReadFile::Ptr cb(
                new AfterReadInReadFile(fd, callback_));
            read(fd, res_, 0, length_, 0, cb);
        }
        return 0;
    }
};

class AfterStatInReadFile : LIBJ_JS_FUNCTION(AfterStatInReadFile)
 private:
    String::CPtr path_;
    JsFunction::Ptr callback_;

 public:
    AfterStatInReadFile(
        String::CPtr path,
        JsFunction::Ptr callback)
        : path_(path)
        , callback_(callback) {}

    Value operator()(JsArray::Ptr args) {
        Int err = -1;
        to<Int>(args->get(0), &err);
        if (err) {
            JsArray::Ptr a = JsArray::create();
            a->add(err);
            (*callback_)(a);
        } else {
            Stats::CPtr stats = toCPtr<Stats>(args->get(1));
            Long size = -1;
            to<Long>(stats->get(STAT_SIZE), &size);
            size = ((size + (1 << 12) - 1) >> 12) << 12;
            Buffer::Ptr res = Buffer::create(size);
            AfterOpenInReadFile::Ptr cb(
                new AfterOpenInReadFile(res, size, callback_));
            open(path_, R, cb);
        }
        return 0;
    }
};

void readFile(String::CPtr path, JsFunction::Ptr callback) {
    if (!callback)
        return;
    AfterStatInReadFile::Ptr cb(new AfterStatInReadFile(path, callback));
    stat(path, cb);
}

}  // namespace fs
}  // namespace node
}  // namespace libj
