CC := gcc
CFLAGS := -O2 -Wall -Wextra -pedantic -fPIC
SRCS := $(wildcard src/*.c)
BIN := bin
LIB := lib
NAME := customlogger
LIB_PATH := $(PWD)/$(LIB)

OBJS := $(SRCS:src/%.c=$(BIN)/%.o)

all: build_shared

build_object:
	$(CC) $(CFLAGS) -c $(SRCS) -o $(OBJS) 

build_shared: build_object
	$(CC) -shared -Wl,-soname,lib$(NAME).so -o $(LIB)/lib$(NAME).so $(OBJS)

setenv:
	@echo 'export LD_LIBRARY_PATH="$(LIB_PATH):$$LD_LIBRARY_PATH"'	

clean:
	rm -r $(BIN)/* && rm -r $(LIB)/*

.PHONY: all build_object build_shared setenv clean
	
