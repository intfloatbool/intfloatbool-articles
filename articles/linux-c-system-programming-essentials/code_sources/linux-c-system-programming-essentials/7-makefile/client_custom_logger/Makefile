SRCS := $(wildcard *.c)
CC := gcc
OUT_NAME := client_custom_logger.out
LINKER_BUILD_TIME := -Llib -lcustomlogger
LINKER_RUNTIME_PARAMS := -Wl,-rpath,'$$ORIGIN/lib'


all:
	$(CC) $(SRCS) $(LINKER_BUILD_TIME) $(LINKER_RUNTIME_PARAMS) -o $(OUT_NAME)

.PHONY: all
	
