#!/bin/bash

make clean


g++ -std=c++17 -O0 -ggdb -DPROTOBUF -Wall -Werror -DRUN_FIXED_DURATION -DFLUSH_CACHE \
    -I../src/net -I../src \
    -o th_benchmark_server.out th_benchmark_server.cpp synthetic.pb.cc \
    `pkg-config --cflags --libs protobuf` \
    -lpthread

g++ -std=c++17 -O0 -ggdb -DPROTOBUF -Wall -Werror -DRUN_FIXED_DURATION -DFLUSH_CACHE \
    -I../src/net -I../src \
    -o th_benchmark_concurrent_client.out th_benchmark_concurrent_client.cpp synthetic.pb.cc \
    `pkg-config --cflags --libs protobuf` \
    -lpthread

#-I/usr/local/include \
    #-L/usr/local/lib \
