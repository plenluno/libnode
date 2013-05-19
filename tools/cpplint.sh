#!/bin/sh
tools/cpplint/cpplint.py --filter=-runtime/explicit,-runtime/threadsafe_fn,-readability/streams,-build/include_what_you_use --prefix=libnode `find include src gtest sample -type f`
