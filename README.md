libnode
=======

[![Build Status](https://travis-ci.org/plenluno/libnode.png?branch=master)](https://travis-ci.org/plenluno/libnode)

libnode is a C++ implementation of Node.js.

Just like Node.js, libnode provides non-blocking I/O,
which enables you to develop scalable Web applications in C++.
libnode is faster and smaller than Node.js,
so it is especially suitable for embedded devices.

### Source Code

You need to clone all the dependant submodules to build libnode.

    git clone --recursive https://github.com/plenluno/libnode.git

### Build

##### Linux/Mac

```sh
mkdir libnode/build
cd libnode/build
cmake ..
make
```

The memory management of libnode is automatic, based on either shared_ptr or bdw-gc.  
libnode uses shared_ptr by default. In order to use bdw-gc,  

    cmake -DLIBNODE_USE_BDWGC=ON ..

If you'd like to post messages to libnode from other threads in your application,

    cmake -DLIBNODE_USE_THREAD=ON ..

##### Windows

    tools/windows-cmake.ps1 [cmake-option ...]

##### Android

    export ANDROID_NDK=/path/to/android-ndk
    tools/android-cmake.sh [cmake-option ...]

##### iOS

    tools/ios-cmake.sh [cmake-option ...]

### Example

The following HTTP server responds with "Hello World" for every request.

```cpp
auto srv = http::Server::create(
    JsClosure::create([] (JsArray::Ptr args) {
        auto res = args->getPtr<http::ServerResponse>(1);
        res->setHeader(http::HEADER_CONTENT_TYPE, str("text/plain"));
        res->end(str("Hello World\n"));
        return UNDEFINED;
    }));
srv->listen(1337, str("127.0.0.1"));
node::run();
```

For more information about the usage, please refer to libnode/gtest.
