# Copyright (c) 2013 Plenluno All rights reserved.

cmake_minimum_required(VERSION 2.8)

project(exts)

# options ----------------------------------------------------------------------

option(LIBNODE_USE_JSDP    "Use JSDP"     ON)
option(LIBNODE_USE_JSONRPC "Use JSON-RPC" ON)

# build libraries --------------------------------------------------------------

include_directories(
    jsdp/include
    jsonrpc/include
)

## jsdp
if(LIBNODE_USE_JSDP)
    add_subdirectory(jsdp)
endif(LIBNODE_USE_JSDP)

## jsonrpc
if(LIBNODE_USE_JSONRPC AND LIBNODE_USE_THREAD)
    add_subdirectory(jsonrpc)
endif(LIBNODE_USE_JSONRPC AND LIBNODE_USE_THREAD)
