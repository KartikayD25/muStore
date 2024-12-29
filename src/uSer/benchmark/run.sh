#!/bin/bash

rm -f resfile
set -e
# LIB:: 0: user, 1: PB, 2: FB
LIB=0
MSG=0
SER=0
if [ $LIB == 0 ]; then
	RUN_DURATION=10 ./th_benchmark_server.out -d ens21np0 -l $LIB -m $MSG -t 1 -s $SER &
	SERVER_PID=$(pgrep -f -n "./th_benchmark_server.out -d ens21np0 -l $LIB -m $MSG -t 1 -s $SER")
else
	RUN_DURATION=10 ./th_benchmark_server.out -d ens21np0 -l $LIB -m $MSG -t 1 &
	SERVER_PID=$(pgrep -f -n "./th_benchmark_server.out -d ens21np0 -l $LIB -m $MSG -t 1")
fi

sudo ./cpu_util.sh 20 "^%Cpu2 " $SERVER_PID resfile
