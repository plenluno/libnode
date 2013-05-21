// Copyright (c) 2013 Plenluno All rights reserved.

#ifndef LIBNODE_DETAIL_FS_H_
#define LIBNODE_DETAIL_FS_H_

#include <libnode/fs.h>
#include <libnode/uv/error.h>
#include <libnode/detail/uv/fs_req.h>

#include <libj/js_array.h>
#include <libj/detail/js_object.h>

#include <assert.h>
#include <fcntl.h>
#include <string.h>

#ifdef LIBJ_PF_WINDOWS
    #define O_SYNC 0
#endif

namespace libj {
namespace node {
namespace detail {
namespace fs {

static const Int INVALID_UID = -1;
static const Int INVALID_GID = -1;

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
        assert(false);
        return -1;
    }
}

static Int convertType(node::fs::Type type) {
    switch (type) {
    case node::fs::FILE:
        return 0;
    case node::fs::DIR:
        return UV_FS_SYMLINK_DIR;
    case node::fs::JUNCTION:
        return UV_FS_SYMLINK_JUNCTION;
    default:
        assert(false);
        return -1;
    }
}

static void onError(uv_fs_t* req) {
    assert(req);
    uv::FsReq* fsReq = static_cast<uv::FsReq*>(req->data);
    if (fsReq && fsReq->onComplete) {
        fsReq->onComplete->call(node::uv::Error::valueOf(req->errorno));
    }
    delete fsReq;
}

static node::fs::Stats::Ptr getStats(uv_fs_t* req) {
    node::fs::Stats::Ptr stats(new libj::detail::JsObject<node::fs::Stats>());
    const struct stat* s = static_cast<const struct stat*>(req->ptr);
    stats->put(node::fs::STAT_DEV,     static_cast<Int>(s->st_dev));
    stats->put(node::fs::STAT_INO,     static_cast<Long>(s->st_ino));
    stats->put(node::fs::STAT_MODE,    static_cast<Int>(s->st_mode));
    stats->put(node::fs::STAT_NLINK,   static_cast<Int>(s->st_nlink));
    stats->put(node::fs::STAT_UID,     static_cast<Int>(s->st_uid));
    stats->put(node::fs::STAT_GID,     static_cast<Int>(s->st_gid));
    stats->put(node::fs::STAT_RDEV,    static_cast<Int>(s->st_rdev));
    stats->put(node::fs::STAT_SIZE,    static_cast<Size>(s->st_size));
    stats->put(node::fs::STAT_ATIME,   JsDate::create(s->st_atime));
    stats->put(node::fs::STAT_MTIME,   JsDate::create(s->st_mtime));
    stats->put(node::fs::STAT_CTIME,   JsDate::create(s->st_ctime));
#ifdef LIBJ_PF_UNIX
    stats->put(node::fs::STAT_BLKSIZE, static_cast<Size>(s->st_blksize));
    stats->put(node::fs::STAT_BLOCKS,  static_cast<Long>(s->st_blocks));
#endif
    return stats;
}

static Buffer::Ptr getBuffer(uv_fs_t* req) {
    uv::FsReq* fsReq = static_cast<uv::FsReq*>(req->data);
    return fsReq->buffer;
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
        case UV_FS_STAT:
        case UV_FS_FSTAT:
        case UV_FS_LSTAT:
            args->add(getStats(req));
            break;
        case UV_FS_READLINK:
            args->add(String::create(req->ptr));
            break;
        case UV_FS_READDIR:
            {
                char* buf = static_cast<char*>(req->ptr);
                Size num = req->result;

                JsArray::Ptr names = JsArray::create();
                for (Size i = 0; i < num; i++) {
                    String::CPtr name = String::create(buf);
                    names->add(name);
                    buf += strlen(buf) + 1;
                }
                args->add(names);
            }
            break;
        case UV_FS_OPEN:
            args->add(static_cast<uv_file>(req->result));
            break;
        case UV_FS_READ:
            args->add(static_cast<Size>(req->result));
            args->add(getBuffer(req));
            break;
        case UV_FS_WRITE:
            args->add(static_cast<Size>(req->result));
            break;
        default:
            break;
        }
        (*(fsReq->onComplete))(args);
    }
    delete fsReq;
}

void stat(String::CPtr path, JsFunction::Ptr callback) {
    uv::FsReq* fsReq = new uv::FsReq(callback);
    if (path) {
        fsReq->path = path->toStdString();
    }

    int r = uv_fs_stat(
        uv_default_loop(),
        &(fsReq->req),
        fsReq->path.c_str(),
        after);
    assert(!r);
}

void fstat(const Value& fd, JsFunction::Ptr callback) {
    uv::FsReq* fsReq = new uv::FsReq(callback);
    to<uv_file>(fd, &(fsReq->file));

    int r = uv_fs_fstat(
        uv_default_loop(),
        &(fsReq->req),
        fsReq->file,
        after);
    assert(!r);
}

void lstat(String::CPtr path, JsFunction::Ptr callback) {
    uv::FsReq* fsReq = new uv::FsReq(callback);
    if (path) {
        fsReq->path = path->toStdString();
    }

    int r = uv_fs_lstat(
        uv_default_loop(),
        &(fsReq->req),
        fsReq->path.c_str(),
        after);
    assert(!r);
}

void chown(
    String::CPtr path,
    const Value& uid,
    const Value& gid,
    JsFunction::Ptr callback) {
    uv::FsReq* fsReq = new uv::FsReq(callback);
    if (path) {
        fsReq->path = path->toStdString();
    }

    Int _uid = INVALID_UID;
    Int _gid = INVALID_GID;
    to<Int>(uid, &_uid);
    to<Int>(gid, &_gid);

    int r = uv_fs_chown(
        uv_default_loop(),
        &(fsReq->req),
        fsReq->path.c_str(),
        _uid,
        _gid,
        after);
    assert(!r);
}

void fchown(
    const Value& fd,
    const Value& uid,
    const Value& gid,
    JsFunction::Ptr callback) {
    uv::FsReq* fsReq = new uv::FsReq(callback);
    to<uv_file>(fd, &(fsReq->file));

    Int _uid = INVALID_UID;
    Int _gid = INVALID_GID;
    to<Int>(uid, &_uid);
    to<Int>(gid, &_gid);

    int r = uv_fs_fchown(
        uv_default_loop(),
        &(fsReq->req),
        fsReq->file,
        _uid,
        _gid,
        after);
    assert(!r);
}

void chmod(
    String::CPtr path,
    UInt mode,
    JsFunction::Ptr callback) {
    uv::FsReq* fsReq = new uv::FsReq(callback);
    if (path) {
        fsReq->path = path->toStdString();
    }

    int r = uv_fs_chmod(
        uv_default_loop(),
        &(fsReq->req),
        fsReq->path.c_str(),
        mode,
        after);
    assert(!r);
}

void fchmod(
    const Value& fd,
    UInt mode,
    JsFunction::Ptr callback) {
    uv::FsReq* fsReq = new uv::FsReq(callback);
    to<uv_file>(fd, &(fsReq->file));

    int r = uv_fs_fchmod(
        uv_default_loop(),
        &(fsReq->req),
        fsReq->file,
        mode,
        after);
    assert(!r);
}

void ftruncate(
    const Value& fd,
    Size len,
    JsFunction::Ptr callback) {
    uv::FsReq* fsReq = new uv::FsReq(callback);
    to<uv_file>(fd, &(fsReq->file));

    int r = uv_fs_ftruncate(
        uv_default_loop(),
        &(fsReq->req),
        fsReq->file,
        len,
        after);
    assert(!r);
}

void utimes(
    String::CPtr path,
    Double atime,
    Double mtime,
    JsFunction::Ptr callback) {
    uv::FsReq* fsReq = new uv::FsReq(callback);
    if (path) {
        fsReq->path = path->toStdString();
    }

    int r = uv_fs_utime(
        uv_default_loop(),
        &(fsReq->req),
        fsReq->path.c_str(),
        atime,
        mtime,
        after);
    assert(!r);
}

void futimes(
    const Value& fd,
    Double atime,
    Double mtime,
    JsFunction::Ptr callback) {
    uv::FsReq* fsReq = new uv::FsReq(callback);
    to<uv_file>(fd, &(fsReq->file));

    int r = uv_fs_futime(
        uv_default_loop(),
        &(fsReq->req),
        fsReq->file,
        atime,
        mtime,
        after);
    assert(!r);
}

void fsync(
    const Value& fd,
    JsFunction::Ptr callback) {
    uv::FsReq* fsReq = new uv::FsReq(callback);
    to<uv_file>(fd, &(fsReq->file));

    int r = uv_fs_fsync(
        uv_default_loop(),
        &(fsReq->req),
        fsReq->file,
        after);
    assert(!r);
}

void mkdir(
    String::CPtr path,
    Int mode,
    JsFunction::Ptr callback) {
    uv::FsReq* fsReq = new uv::FsReq(callback);
    if (path) {
        fsReq->path = path->toStdString();
    }

    int r = uv_fs_mkdir(
        uv_default_loop(),
        &(fsReq->req),
        fsReq->path.c_str(),
        mode,
        after);
    assert(!r);
}

void rmdir(
    String::CPtr path,
    JsFunction::Ptr callback) {
    uv::FsReq* fsReq = new uv::FsReq(callback);
    if (path) {
        fsReq->path = path->toStdString();
    }

    int r = uv_fs_rmdir(
        uv_default_loop(),
        &(fsReq->req),
        fsReq->path.c_str(),
        after);
    assert(!r);
}

void readdir(
    String::CPtr path,
    JsFunction::Ptr callback) {
    uv::FsReq* fsReq = new uv::FsReq(callback);
    if (path) {
        fsReq->path = path->toStdString();
    }

    int r = uv_fs_readdir(
        uv_default_loop(),
        &(fsReq->req),
        fsReq->path.c_str(),
        0,
        after);
    assert(!r);
}

void rename(
    String::CPtr oldPath,
    String::CPtr newPath,
    JsFunction::Ptr callback) {
    uv::FsReq* fsReq = new uv::FsReq(callback);
    Size secondStart = 1;
    if (oldPath) {
        fsReq->path = oldPath->toStdString();
        secondStart += fsReq->path.length();
    }
    fsReq->path += '\0';
    if (newPath) {
        fsReq->path += newPath->toStdString();
    }

    const char* _oldPath = fsReq->path.c_str();
    const char* _newPath = _oldPath + secondStart;
    int r = uv_fs_rename(
        uv_default_loop(),
        &(fsReq->req),
        _oldPath,
        _newPath,
        after);
    assert(!r);
}

void link(
    String::CPtr srcPath,
    String::CPtr dstPath,
    JsFunction::Ptr callback) {
    uv::FsReq* fsReq = new uv::FsReq(callback);
    Size secondStart = 1;
    if (srcPath) {
        fsReq->path = srcPath->toStdString();
        secondStart += fsReq->path.length();
    }
    fsReq->path += '\0';
    if (dstPath) {
        fsReq->path += dstPath->toStdString();
    }

    const char* _srcPath = fsReq->path.c_str();
    const char* _dstPath = _srcPath + secondStart;
    int r = uv_fs_link(
        uv_default_loop(),
        &(fsReq->req),
        _srcPath,
        _dstPath,
        after);
    assert(!r);
}

void symlink(
    String::CPtr srcPath,
    String::CPtr dstPath,
    node::fs::Type type,
    JsFunction::Ptr callback) {
    uv::FsReq* fsReq = new uv::FsReq(callback);
    Size secondStart = 1;
    if (srcPath) {
        fsReq->path = srcPath->toStdString();
        secondStart += fsReq->path.length();
    }
    fsReq->path += '\0';
    if (dstPath) {
        fsReq->path += dstPath->toStdString();
    }

    const char* _srcPath = fsReq->path.c_str();
    const char* _dstPath = _srcPath + secondStart;
    int r = uv_fs_symlink(
        uv_default_loop(),
        &(fsReq->req),
        _srcPath,
        _dstPath,
        convertType(type),
        after);
    assert(!r);
}

void unlink(
    String::CPtr path,
    JsFunction::Ptr callback) {
    uv::FsReq* fsReq = new uv::FsReq(callback);
    if (path) {
        fsReq->path = path->toStdString();
    }

    int r = uv_fs_unlink(
        uv_default_loop(),
        &(fsReq->req),
        fsReq->path.c_str(),
        after);
    assert(!r);
}

void readlink(
    String::CPtr path,
    JsFunction::Ptr callback) {
    uv::FsReq* fsReq = new uv::FsReq(callback);
    if (path) {
        fsReq->path = path->toStdString();
    }

    int r = uv_fs_readlink(
        uv_default_loop(),
        &(fsReq->req),
        fsReq->path.c_str(),
        after);
    assert(!r);
}

void open(
    String::CPtr path,
    node::fs::Flag flag,
    UInt mode,
    JsFunction::Ptr callback) {
    uv::FsReq* fsReq = new uv::FsReq(callback);
    if (path) {
        fsReq->path = path->toStdString();
    }

    int r = uv_fs_open(
        uv_default_loop(),
        &(fsReq->req),
        fsReq->path.c_str(),
        convertFlag(flag),
        static_cast<int>(mode),
        after);
    assert(!r);
}

void close(
    const Value& fd,
    JsFunction::Ptr callback) {
    uv::FsReq* fsReq = new uv::FsReq(callback);
    to<uv_file>(fd, &(fsReq->file));

    int r = uv_fs_close(
        uv_default_loop(),
        &(fsReq->req),
        fsReq->file,
        after);
    assert(!r);
}

void read(
    const Value &fd,
    Buffer::Ptr buffer,
    Size offset,
    Size length,
    Size position,
    JsFunction::Ptr callback) {
    Size bufLen = buffer ? buffer->length() : 0;
    Size len;
    if (bufLen > offset) {
        len = bufLen - offset;
        len = len < length ? len : length;
    } else {
        len = 0;
    }

    uv::FsReq* fsReq = new uv::FsReq(callback);
    to<uv_file>(fd, &(fsReq->file));
    fsReq->buffer = buffer;

    void* buf = buffer ? const_cast<void*>(buffer->data()) : NULL;
    int r = uv_fs_read(
        uv_default_loop(),
        &(fsReq->req),
        fsReq->file,
        buf,
        len,
        position,
        after);
    assert(!r);
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
        if (err) {
            assert(args->length() == 1);
        } else {
            args->remove(1);
            assert(args->length() == 2);
        }
        if (callback_) (*callback_)(args);
        close(fd_, JsFunction::null());
        return Status::OK;
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
            assert(args->length() == 1);
            if (callback_) (*callback_)(args);
        } else {
            Value fd = args->get(1);
            JsFunction::Ptr cb(new AfterReadInReadFile(fd, callback_));
            fs::read(fd, res_, 0, length_, 0, cb);
        }
        return Status::OK;
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
            assert(args->length() == 1);
            if (callback_) (*callback_)(args);
        } else {
            node::fs::Stats::CPtr stats =
                toCPtr<node::fs::Stats>(args->get(1));
            Size size = 0;
            to<Size>(stats->get(node::fs::STAT_SIZE), &size);
            Buffer::Ptr res = Buffer::create(size);
            JsFunction::Ptr cb(new AfterOpenInReadFile(res, size, callback_));
            fs::open(path_, node::fs::R, 438, cb);
        }
        return Status::OK;
    }

 private:
    String::CPtr path_;
    JsFunction::Ptr callback_;
};

void readFile(String::CPtr path, JsFunction::Ptr callback) {
    JsFunction::Ptr cb(new AfterStatInReadFile(path, callback));
    stat(path, cb);
}

void write(
    const Value& fd,
    Buffer::Ptr buffer,
    Size offset,
    Size length,
    Size position,
    JsFunction::Ptr callback) {
    Size bufLen = buffer ? buffer->length() : 0;
    Size len;
    if (bufLen > offset) {
        len = bufLen - offset;
        len = len < length ? len : length;
    } else {
        len = 0;
    }

    uv::FsReq* fsReq = new uv::FsReq(callback);
    to<uv_file>(fd, &(fsReq->file));
    fsReq->buffer = buffer;

    void* buf = buffer ? const_cast<void*>(buffer->data()) : NULL;
    int r = uv_fs_write(
        uv_default_loop(),
        &(fsReq->req),
        fsReq->file,
        buf,
        len,
        position,
        after);
    assert(!r);
}

void writeAll(
    const Value& fd,
    Buffer::Ptr buffer,
    Size offset,
    Size length,
    Size position,
    JsFunction::Ptr callback);

class AfterCloseInWriteAll : LIBJ_JS_FUNCTION(AfterCloseInWriteAll)
 public:
    AfterCloseInWriteAll(
        JsFunction::Ptr callback,
        Error::CPtr err)
        : callback_(callback)
        , err_(err) {}

    virtual Value operator()(JsArray::Ptr args) {
        args->clear();
        args->add(err_);
        if (callback_) (*callback_)(args);
        return Status::OK;
    }

 private:
    JsFunction::Ptr callback_;
    Error::CPtr err_;
};

class AfterWriteInWriteAll : LIBJ_JS_FUNCTION(AfterWriteInWriteAll)
 public:
    AfterWriteInWriteAll(
        const Value& fd,
        Buffer::Ptr buffer,
        Size offset,
        Size length,
        Size position,
        JsFunction::Ptr callback)
        : fd_(fd)
        , buffer_(buffer)
        , offset_(offset)
        , length_(length)
        , position_(position)
        , callback_(callback) {}

    virtual Value operator()(JsArray::Ptr args) {
        Error::CPtr err = args->getCPtr<Error>(0);
        if (err) {
            JsFunction::Ptr cb(new AfterCloseInWriteAll(callback_, err));
            close(fd_, cb);
        } else {
            Size written = 0;
            to<Size>(args->get(1), &written);
            if (written == length_) {
                close(fd_, callback_);
            } else {
                offset_ += written;
                length_ -= written;
                position_ += written;
                writeAll(fd_, buffer_, offset_, length_, position_, callback_);
            }
        }
        return Status::OK;
    }

 private:
    Value fd_;
    Buffer::Ptr buffer_;
    Size offset_;
    Size length_;
    Size position_;
    JsFunction::Ptr callback_;
};

void writeAll(
    const Value& fd,
    Buffer::Ptr buffer,
    Size offset,
    Size length,
    Size position,
    JsFunction::Ptr callback) {
    JsFunction::Ptr cb(
        new AfterWriteInWriteAll(
            fd, buffer, offset, length, position, callback));
    write(fd, buffer, offset, length, position, cb);
}

class AfterOpenInWriteFile : LIBJ_JS_FUNCTION(AfterOpenInWriteFile)
 public:
    AfterOpenInWriteFile(
        Buffer::Ptr data,
        JsFunction::Ptr callback)
        : data_(data)
        , callback_(callback) {}

    virtual Value operator()(JsArray::Ptr args) {
        Error::CPtr err = args->getCPtr<Error>(0);
        if (err) {
            assert(args->length() == 1);
            if (callback_) (*callback_)(args);
        } else {
            Value fd = args->get(1);
            Size len = data_ ? data_->length() : 0;
            writeAll(fd, data_, 0, len, 0, callback_);
        }
        return Status::OK;
    }

 private:
    Buffer::Ptr data_;
    JsFunction::Ptr callback_;
};

void writeFile(
    String::CPtr path,
    Buffer::Ptr data,
    JsFunction::Ptr callback) {
    JsFunction::Ptr cb(new AfterOpenInWriteFile(data, callback));
    fs::open(path, node::fs::W, 438, cb);
}

class AfterOpenInAppendFile : LIBJ_JS_FUNCTION(AfterOpenInAppendFile)
 public:
    AfterOpenInAppendFile(
        Buffer::Ptr data,
        JsFunction::Ptr callback)
        : data_(data)
        , callback_(callback) {}

    virtual Value operator()(JsArray::Ptr args) {
        Error::CPtr err = args->getCPtr<Error>(0);
        if (err) {
            assert(args->length() == 1);
            if (callback_) (*callback_)(args);
        } else {
            Value fd = args->get(1);
            Size len = data_ ? data_->length() : 0;
            writeAll(fd, data_, 0, len, NO_POS, callback_);
        }
        return Status::OK;
    }

 private:
    Buffer::Ptr data_;
    JsFunction::Ptr callback_;
};

void appendFile(
    String::CPtr path,
    Buffer::Ptr data,
    JsFunction::Ptr callback) {
    JsFunction::Ptr cb(new AfterOpenInAppendFile(data, callback));
    fs::open(path, node::fs::A, 438, cb);
}

class AfterCloseInTruncate : LIBJ_JS_FUNCTION(AfterCloseInTruncate)
 public:
    AfterCloseInTruncate(
        Error::CPtr err,
        JsFunction::Ptr cb)
        : err_(err)
        , callback_(cb) {}

    virtual Value operator()(JsArray::Ptr args) {
        assert(args->length() == 1);
        if (err_) args->set(1, err_);
        (*callback_)(args);
        return Status::OK;
    }

 private:
    Error::CPtr err_;
    JsFunction::Ptr callback_;
};

class AfterFtruncateInTruncate : LIBJ_JS_FUNCTION(AfterFtruncateInTruncate)
 public:
    AfterFtruncateInTruncate(
        const Value& fd,
        JsFunction::Ptr cb)
        : fd_(fd)
        , callback_(cb) {}

    virtual Value operator()(JsArray::Ptr args) {
        Error::CPtr err = args->getCPtr<Error>(0);
        JsFunction::Ptr cb(new AfterCloseInTruncate(err, callback_));
        fs::close(fd_, cb);
        return Status::OK;
    }

 private:
    Value fd_;
    JsFunction::Ptr callback_;
};

class  AfterOpenInTruncate : LIBJ_JS_FUNCTION(AfterOpenInTruncate)
 public:
    AfterOpenInTruncate(
        Size len,
        JsFunction::Ptr cb)
        : len_(len)
        , callback_(cb) {}

    virtual Value operator()(JsArray::Ptr args) {
        Error::CPtr err = args->getCPtr<Error>(0);
        if (err) {
            assert(args->length() == 1);
            if (callback_) (*callback_)(args);
        } else {
            Value fd = args->get(1);
            JsFunction::Ptr cb(new AfterFtruncateInTruncate(fd, callback_));
            fs::ftruncate(fd, len_, cb);
        }
        return Status::OK;
    }

 private:
    Size len_;
    JsFunction::Ptr callback_;
};

void truncate(
    String::CPtr path,
    Size len,
    JsFunction::Ptr callback) {
    AfterOpenInTruncate::Ptr cb(new AfterOpenInTruncate(len, callback));
    fs::open(path, node::fs::W, 438, cb);
}

}  // namespace fs
}  // namespace detail
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_DETAIL_FS_H_
