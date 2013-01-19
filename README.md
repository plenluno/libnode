libnode
=======

libnode is a native implementation of Node.js.

### Build

    git clone --recursive https://github.com/plenluno/libnode.git
    mkdir libnode/build
    cd libnode/build
    cmake ..
    make

If g++ supports C++11 in your environment,

    cmake -DLIBNODE_USE_CXX11=ON ..

### Example

    http::Server::Ptr srv = http::Server::create(
        JsClosure::create([] (JsArray::Ptr args) -> Value {
            http::ServerResponse::Ptr res =
                args->getPtr<http::ServerResponse>(1);
            res->setHeader(
                http::HEADER_CONTENT_TYPE,
                String::create("text/plain"));
            res->end(String::create("Hello World\n"));
            return Status::OK;
        }));
    srv->listen(1337, String::create("127.0.0.1"));
    node::run();

For more information about the usage, please refer to libnode/gtest.
