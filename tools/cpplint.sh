#!/bin/sh
tools/cpplint/cpplint.py --filter=-runtime/explicit,-readability/streams --prefix=libnode `find include src gtest sample -type f`
