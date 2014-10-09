include /usr/local/systemc-2.3.1/examples/sysc/Makefile.config

PROJECT := filter
SRCS    := $(wildcard *.cpp)
OBJS    := $(SRCS:.cpp=.o)

include /usr/local/systemc-2.3.1/examples/sysc/Makefile.rules
