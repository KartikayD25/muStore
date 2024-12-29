SRCC=$(wildcard *.c)
CTARGETS=$(patsubst %.c,%,$(SRCC))

SRCCPP=$(wildcard *.cpp)
CPPTARGETS=$(patsubst %.cpp,%,$(SRCCPP))

default: $(CTARGETS)

OPT=-O0 -ggdb
#OPT=-O0 -ggdb
#OPT=-O2

#FLAGS += -DPROTOBUF
#FLAGS += synthetic.pb.cc

FLAGS += -Wall -Werror
FLAGS += -DRUN_FIXED_DURATION
FLAGS += -DFLUSH_CACHE
FLAGS += -I../src/net
FLAGS += -I../src
LIBS += -pthread

%: %.c
	gcc -std=c17 $(FLAGS) $(OPT) $(LIBS) \
	        -o $*.out $<

default: $(CPPTARGETS)

%: %.cpp
	g++ -std=c++17 $(FLAGS) $(OPT) $(LIBS) \
                -o $*.out $<

format:
	# Google style
	clang-format -i *.c *.h *.cpp *.hpp

protoc:
	protoc -I=./proto --cpp_out=.  proto/synthetic.proto
flatc:
	flatc --cpp proto/synthetic.fbs

protoc-clean:
	rm -r *.pb.*

clean:
	rm -f *.out

runbmserver:
	#sudo ip netns exec ns1 ./sg_benchmark.out -s
	sudo ip netns exec ns1 ./th_benchmark_server.out -s
runbmclient:
	sudo ip netns exec ns2 ./th_benchmark_server.out -c 10.0.1.1
