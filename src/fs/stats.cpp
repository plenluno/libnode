// Copyright (c) 2012 Plenluno All rights reserved.

#include "libnode/fs/stats.h"

namespace libj {
namespace node {
namespace fs {

const String::CPtr STAT_DEV = String::intern("dev");
const String::CPtr STAT_INO = String::intern("ino");
const String::CPtr STAT_MODE = String::intern("mode");
const String::CPtr STAT_NLINK = String::intern("nlink");
const String::CPtr STAT_UID = String::intern("uid");
const String::CPtr STAT_GID = String::intern("gid");
const String::CPtr STAT_RDEV = String::intern("rdev");
const String::CPtr STAT_SIZE = String::intern("size");
const String::CPtr STAT_BLKSIZE = String::intern("blksize");
const String::CPtr STAT_BLOCKS = String::intern("blocks");
const String::CPtr STAT_ATIME = String::intern("atime");
const String::CPtr STAT_MTIME = String::intern("mtime");
const String::CPtr STAT_CTIME = String::intern("ctime");

}  // namespace events
}  // namespace node
}  // namespace libj
