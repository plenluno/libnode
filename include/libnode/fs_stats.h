// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_FS_STATS_H_
#define LIBNODE_FS_STATS_H_

#include <libj/js_object.h>
#include <libj/string.h>

namespace libj {
namespace node {
namespace fs {

extern const String::CPtr STAT_DEV;
extern const String::CPtr STAT_INO;
extern const String::CPtr STAT_MODE;
extern const String::CPtr STAT_NLINK;
extern const String::CPtr STAT_UID;
extern const String::CPtr STAT_GID;
extern const String::CPtr STAT_RDEV;
extern const String::CPtr STAT_SIZE;
extern const String::CPtr STAT_BLKSIZE;
extern const String::CPtr STAT_BLOCKS;
extern const String::CPtr STAT_ATIME;
extern const String::CPtr STAT_MTIME;
extern const String::CPtr STAT_CTIME;

class Stats : LIBJ_JS_OBJECT(Stats)
 public:

};

}  // namespace fs
}  // namespace node
}  // namespace libj

#endif  // LIBNODE_FS_STATS_H_
