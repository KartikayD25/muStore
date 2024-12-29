SRC = $(wildcard *.cpp ./xperiment/*.cpp) 
targets = $(patsubst %.cpp,%,$(SRC))


uSer="/home/sid/uSer/src"
#OPT = -O3
OPT = -O0 -ggdb

CC = g++
#FLAGS = $(OPT) -Wall -Werror -std=c++11 -pthread
FLAGS = $(OPT) -std=c++17 -pthread
FLAGS += -DTEST_USER

default: $(targets)

%: %.cpp
	$(CC) $(FLAGS) -I$(uSer) -o $*.out $<

clean:
	rm -f *.out
format:
	clang-format -i $(SRC)
