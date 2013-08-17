libnode
=======

[![Build Status](https://travis-ci.org/plenluno/libnode.png?branch=master)](https://travis-ci.org/plenluno/libnode)

libnode is a native implementation of Node.js.

### Build

    git clone --recursive https://github.com/plenluno/libnode.git
    mkdir libnode/build
    cd libnode/build
    cmake ..
    make

The memory management of libnode is automatic, based on either shared_ptr or bdw-gc.  
libnode uses shared_ptr by default. In order to use bdw-gc,  

    cmake -DLIBNODE_USE_BDWGC=ON ..

If your compiler supports C++11,

    cmake -DLIBNODE_USE_CXX11=ON ..

It allows you to use closures like the example below.

### Example

The following HTTP server responds with "Hello World" for every request.

    auto srv = http::Server::create(
        JsClosure::create([] (JsArray::Ptr args) {
            auto res = args->getPtr<http::ServerResponse>(1);
            res->setHeader(http::HEADER_CONTENT_TYPE, str("text/plain"));
            res->end(str("Hello World\n"));
            return UNDEFINED;
        }));
    srv->listen(1337, str("127.0.0.1"));
    node::run();

For more information about the usage, please refer to libnode/gtest.
