#!/bin/bash

make clean
#flatc --cpp proto/synthetic.fbs

g++ -std=c++17 -O0 -ggdb -DFLATBUF -Wall -Werror -DRUN_FIXED_DURATION -DFLUSH_CACHE \
    -I../src/net -I../src \
    -o th_benchmark_concurrent_client.out th_benchmark_concurrent_client.cpp \
    -lpthread
    #`pkg-config --cflags --libs protobuf` \

g++ -std=c++17 -O0 -ggdb -DFLATBUF -Wall -Werror -DRUN_FIXED_DURATION -DFLUSH_CACHE \
    -I../src/net -I../src \
    -o th_benchmark_server.out th_benchmark_server.cpp \
    -lpthread

#-I/usr/local/include \
    #-L/usr/local/lib \
