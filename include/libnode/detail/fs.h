// Copyright (c) 2013 Plenluno All rights reserved.

#ifndef LIBNODE_DETAIL_FS_H_
#define LIBNODE_DETAIL_FS_H_

#include <libnode/fs.h>
#include <libnode/uv/error.h>
#include <libnode/detail/uv/fs_read.h>

#include <libj/js_array.h>
#include <libj/detail/js_object.h>

#include <fcntl.h>

namespace libj {
namespace node {
namespace detail {
namespace fs {

static Int convertFlag(node::fs::Flag flag) {
    switch (flag) {
    case node::fs::R:
        return O_RDONLY;
    case node::fs::RS:
        return O_RDONLY | O_SYNC;
    case node::fs::RP:
        return O_RDWR;
    case node::fs::RSP:
        return O_RDWR | O_SYNC;
    case node::fs::W:
        return O_TRUNC | O_CREAT | O_WRONLY;
    case node::fs::WX:
        return O_TRUNC | O_CREAT | O_WRONLY | O_EXCL;
    case node::fs::WP:
        return O_TRUNC | O_CREAT | O_RDWR;
    case node::fs::WXP:
        return O_TRUNC | O_CREAT | O_RDWR | O_EXCL;
    case node::fs::A:
        return O_APPEND | O_CREAT | O_WRONLY;
    case node::fs::AX:
        return O_APPEND | O_CREAT | O_WRONLY | O_EXCL;
    case node::fs::AP:
        return O_APPEND | O_CREAT | O_RDWR;
    case node::fs::AXP:
        return O_APPEND | O_CREAT | O_RDWR | O_EXCL;
    default:
        return -1;
    }
}

static void onError(uv_fs_t* req) {
    uv::FsReq* fsReq = static_cast<uv::FsReq*>(req->data);
    if (fsReq->onComplete) {
        fsReq->onComplete->call(node::uv::Error::valueOf(req->errorno));
    }
    delete fsReq;
}

static node::fs::Stats::Ptr getStats(uv_fs_t* req) {
    node::fs::Stats::Ptr stats(new libj::detail::JsObject<node::fs::Stats>());
    const struct stat* s = static_cast<const struct stat*>(req->ptr);
    stats->put(node::fs::STAT_DEV,     static_cast<Long>(s->st_dev));
    stats->put(node::fs::STAT_INO,     static_cast<Long>(s->st_ino));
    stats->put(node::fs::STAT_MODE,    static_cast<Long>(s->st_mode));
    stats->put(node::fs::STAT_NLINK,   static_cast<Long>(s->st_nlink));
    stats->put(node::fs::STAT_UID,     static_cast<Long>(s->st_uid));
    stats->put(node::fs::STAT_GID,     static_cast<Long>(s->st_gid));
    stats->put(node::fs::STAT_RDEV,    static_cast<Long>(s->st_rdev));
    stats->put(node::fs::STAT_SIZE,    static_cast<Long>(s->st_size));
    stats->put(node::fs::STAT_BLKSIZE, static_cast<Long>(s->st_blksize));
    stats->put(node::fs::STAT_BLOCKS,  static_cast<Long>(s->st_blocks));
    stats->put(node::fs::STAT_ATIME,   static_cast<Long>(s->st_atime));
    stats->put(node::fs::STAT_MTIME,   static_cast<Long>(s->st_mtime));
    stats->put(node::fs::STAT_CTIME,   static_cast<Long>(s->st_ctime));
    return stats;
}

static Buffer::Ptr getBuffer(uv_fs_t* req) {
    uv::FsRead* fsRead = static_cast<uv::FsRead*>(req->data);
    Size offset = fsRead->offset;
    Size bytesRead = req->result;
    char* buf = fsRead->buffer;
    Buffer::Ptr res = fsRead->res;
    for (Size i = 0; i < bytesRead; i++) {
        res->writeUInt8(buf[i], offset + i);
    }
    return res;
}

static void after(uv_fs_t* req) {
    if (req->result == -1) {
        onError(req);
        return;
    }

    uv::FsReq* fsReq = static_cast<uv::FsReq*>(req->data);
    if (fsReq->onComplete) {
        JsArray::Ptr args = JsArray::create();
        args->add(Error::null());
        switch (req->fs_type) {
        case UV_FS_OPEN:
            args->add(static_cast<uv_file>(req->result));
            break;
        case UV_FS_READ:
            args->add(static_cast<Size>(req->result));
            args->add(getBuffer(req));
            break;
        case UV_FS_STAT:
            args->add(getStats(req));
            break;
        case UV_FS_CLOSE:
        default:
            break;
        }
        (*(fsReq->onComplete))(args);
    }
    delete fsReq;
}

void open(
    String::CPtr path,
    node::fs::Flag flag,
    UInt mode,
    JsFunction::Ptr callback) {
    uv::FsReq* fsReq = new uv::FsReq(callback);
    fsReq->path = path->toStdString();
    uv_fs_t* req = &(fsReq->req);
    int r = uv_fs_open(
        uv_default_loop(),
        req,
        fsReq->path.c_str(),
        convertFlag(flag),
        static_cast<int>(mode),
        after);
    if (r < 0) {
        req->errorno = uv_last_error(uv_default_loop()).code;
        onError(req);
    }
}

void close(
    const Value& fd,
    JsFunction::Ptr callback) {
    uv::FsReq* fsReq = new uv::FsReq(callback);
    uv_fs_t* req = &(fsReq->req);
    if (!to<uv_file>(fd, &(fsReq->file))) {
        req->errorno = UV_EINVAL;
        onError(req);
        return;
    }
    int r = uv_fs_close(
        uv_default_loop(),
        req,
        fsReq->file,
        after);
    if (r < 0) {
        req->errorno = uv_last_error(uv_default_loop()).code;
        onError(req);
    }
}

void read(
    const Value &fd,
    Buffer::Ptr buffer,
    Size offset,
    Size length,
    Size position,
    JsFunction::Ptr callback) {
    Size len = 0;
    Size bufLen = buffer ? buffer->length() : 0;
    if (length > 0 && offset < bufLen) {
        len = bufLen - offset;
        len = len < length ? len : length;
    }
    uv::FsRead* fsRead = new uv::FsRead(len, callback);
    uv_fs_t* req = &(fsRead->req);
    if (!len) {
        req->errorno = UV_EINVAL;
        onError(req);
        return;
    }

    if (!to<uv_file>(fd, &(fsRead->file))) {
        req->errorno = UV_EINVAL;
        onError(req);
        return;
    }
    fsRead->res = buffer;
    fsRead->offset = offset;

    int r = uv_fs_read(
        uv_default_loop(),
        req,
        fsRead->file,
        fsRead->buffer,
        len,
        position,
        after);
    if (r < 0) {
        req->errorno = uv_last_error(uv_default_loop()).code;
        onError(req);
    }
}

void stat(String::CPtr path, JsFunction::Ptr callback) {
    uv::FsReq* fsReq = new uv::FsReq(callback);
    if (path)
        fsReq->path = path->toStdString();
    uv_fs_t* req = &(fsReq->req);
    int r = uv_fs_stat(
        uv_default_loop(),
        req,
        fsReq->path.c_str(),
        after);
    if (r < 0) {
        req->errorno = uv_last_error(uv_default_loop()).code;
        onError(req);
    }
}

class AfterReadInReadFile : LIBJ_JS_FUNCTION(AfterReadInReadFile)
 public:
    AfterReadInReadFile(
        const Value& fd,
        JsFunction::Ptr callback)
        : fd_(fd)
        , callback_(callback) {}

    virtual Value operator()(JsArray::Ptr args) {
        Error::CPtr err = args->getCPtr<Error>(0);
        JsArray::Ptr a = JsArray::create();
        a->add(err);
        if (!err) {
            a->add(args->get(2));
        }
        (*callback_)(a);
        close(fd_, JsFunction::null());
        return 0;
    }

 private:
    Value fd_;
    JsFunction::Ptr callback_;
};

class AfterOpenInReadFile : LIBJ_JS_FUNCTION(AfterOpenInReadFile)
 public:
    AfterOpenInReadFile(
        Buffer::Ptr res,
        Size len,
        JsFunction::Ptr callback)
        : res_(res)
        , length_(len)
        , callback_(callback) {}

    virtual Value operator()(JsArray::Ptr args) {
        Error::CPtr err = args->getCPtr<Error>(0);
        if (err) {
            JsArray::Ptr a = JsArray::create();
            a->add(err);
            (*callback_)(a);
        } else {
            Value fd = args->get(1);
            AfterReadInReadFile::Ptr cb(
                new AfterReadInReadFile(fd, callback_));
            fs::read(fd, res_, 0, length_, 0, cb);
        }
        return 0;
    }

 private:
    Buffer::Ptr res_;
    Size length_;
    JsFunction::Ptr callback_;
};

class AfterStatInReadFile : LIBJ_JS_FUNCTION(AfterStatInReadFile)
 public:
    AfterStatInReadFile(
        String::CPtr path,
        JsFunction::Ptr callback)
        : path_(path)
        , callback_(callback) {}

    virtual Value operator()(JsArray::Ptr args) {
        Error::CPtr err = args->getCPtr<Error>(0);
        if (err) {
            JsArray::Ptr a = JsArray::create();
            a->add(err);
            (*callback_)(a);
        } else {
            node::fs::Stats::CPtr stats =
                toCPtr<node::fs::Stats>(args->get(1));
            Long size = -1;
            to<Long>(stats->get(node::fs::STAT_SIZE), &size);
            Buffer::Ptr res = Buffer::create(size);
            AfterOpenInReadFile::Ptr cb(
                new AfterOpenInReadFile(res, size, callback_));
            fs::open(path_, node::fs::R, 438, cb);
        }
        return 0;
    }

 private:
    String::CPtr path_;
    JsFunction::Ptr callback_;
};

void readFile(String::CPtr path, JsFunction::Ptr callback) {
    AfterStatInReadFile::Ptr cb(new AfterStatInReadFile(path, callback));
    stat(path, cb);
}

}  // namespace fs
}  // namespace detail
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_DETAIL_FS_H_
