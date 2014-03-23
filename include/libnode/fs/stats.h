// Copyright (c) 2012-2014 Plenluno All rights reserved.

#ifndef LIBNODE_FS_STATS_H_
#define LIBNODE_FS_STATS_H_

#include <libj/js_object.h>
#include <libj/symbol.h>

namespace libj {
namespace node {
namespace fs {

extern Symbol::CPtr STAT_DEV;
extern Symbol::CPtr STAT_INO;
extern Symbol::CPtr STAT_MODE;
extern Symbol::CPtr STAT_NLINK;
extern Symbol::CPtr STAT_UID;
extern Symbol::CPtr STAT_GID;
extern Symbol::CPtr STAT_RDEV;
extern Symbol::CPtr STAT_SIZE;
extern Symbol::CPtr STAT_BLKSIZE;
extern Symbol::CPtr STAT_BLOCKS;
extern Symbol::CPtr STAT_ATIME;
extern Symbol::CPtr STAT_MTIME;
extern Symbol::CPtr STAT_CTIME;

class Stats : LIBJ_JS_OBJECT(Stats)
 public:
    virtual Boolean isFile() const = 0;

    virtual Boolean isDirectory() const = 0;

    virtual Boolean isBlockDevice() const = 0;

    virtual Boolean isCharacterDevice() const = 0;

    virtual Boolean isSymbolicLink() const = 0;

    virtual Boolean isFIFO() const = 0;

    virtual Boolean isSocket() const = 0;
};

}  // namespace fs
}  // namespace node
}  // namespace libj

#include <libnode/impl/fs/stats.h>

#endif  // LIBNODE_FS_STATS_H_
