#!/bin/sh
tools/cpplint/cpplint.py --filter=-runtime/explicit,-readability/streams include/libnode/* src/* gtest/* sample/*
