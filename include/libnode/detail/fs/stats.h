// Copyright (c) 2013-2015 Plenluno All rights reserved.

#ifndef LIBNODE_DETAIL_FS_STATS_H_
#define LIBNODE_DETAIL_FS_STATS_H_

#include <libnode/fs/stats.h>

#include <libj/js_date.h>
#include <libj/detail/js_object.h>

#include <uv.h>

namespace libj {
namespace node {
namespace detail {
namespace fs {

#define LIBNODE_UV_STAT_TIME_TO_JS_DATE(T) \
    JsDate::create( \
        static_cast<Double>(T.tv_sec * 1000) + \
        static_cast<Double>(T.tv_nsec / 1000000))

class Stats : public libj::detail::JsObject<node::fs::Stats> {
 public:
    LIBJ_MUTABLE_DEFS(Stats, LIBNODE_FS_STATS);

    Stats(const uv_stat_t* s)
        : mode_(static_cast<Int>(s->st_mode)) {
        put(node::fs::STAT_DEV,     static_cast<Int>(s->st_dev));
        put(node::fs::STAT_INO,     static_cast<Long>(s->st_ino));
        put(node::fs::STAT_MODE,    static_cast<Int>(s->st_mode));
        put(node::fs::STAT_NLINK,   static_cast<Int>(s->st_nlink));
        put(node::fs::STAT_UID,     static_cast<Int>(s->st_uid));
        put(node::fs::STAT_GID,     static_cast<Int>(s->st_gid));
        put(node::fs::STAT_RDEV,    static_cast<Int>(s->st_rdev));
        put(node::fs::STAT_SIZE,    static_cast<Size>(s->st_size));
#ifdef LIBJ_PF_UNIX
        put(node::fs::STAT_BLKSIZE, static_cast<Size>(s->st_blksize));
        put(node::fs::STAT_BLOCKS,  static_cast<Long>(s->st_blocks));
#endif
        put(node::fs::STAT_ATIME,
            LIBNODE_UV_STAT_TIME_TO_JS_DATE(s->st_atim));
        put(node::fs::STAT_MTIME,
            LIBNODE_UV_STAT_TIME_TO_JS_DATE(s->st_mtim));
        put(node::fs::STAT_CTIME,
            LIBNODE_UV_STAT_TIME_TO_JS_DATE(s->st_ctim));
    }

    virtual Boolean isFile() const {
        return (mode_ & S_IFMT) == S_IFREG;
    }

    virtual Boolean isDirectory() const {
        return (mode_ & S_IFMT) == S_IFDIR;
    }

    virtual Boolean isBlockDevice() const {
#ifdef LIBJ_PF_UNIX
        return (mode_ & S_IFMT) == S_IFBLK;
#else
        return false;
#endif
    }

    virtual Boolean isCharacterDevice() const {
        return (mode_ & S_IFMT) == S_IFCHR;
    }

    virtual Boolean isSymbolicLink() const {
        return (mode_ & S_IFMT) == S_IFLNK;
    }

    virtual Boolean isFIFO() const {
#ifdef LIBJ_PF_UNIX
        return (mode_ & S_IFMT) == S_IFIFO;
#else
        return false;
#endif
    }

    virtual Boolean isSocket() const {
#ifdef LIBJ_PF_UNIX
        return (mode_ & S_IFMT) == S_IFSOCK;
#else
        return false;
#endif
    }

 private:
    Int mode_;
};

}  // namespace fs
}  // namespace detail
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_DETAIL_FS_STATS_H_
