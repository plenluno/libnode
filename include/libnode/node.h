// Copyright (c) 2012 Plenluno All rights reserved.

#ifndef LIBNODE_NODE_H_
#define LIBNODE_NODE_H_

#include "libj/singleton.h"

namespace libj {
namespace node {

class Node : LIBJ_SINGLETON(Node)
 public:
    void run();
};

}  // namespace node
}  // namespace libj

#endif  // LIBNODE_URL_H_