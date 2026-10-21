CC ?= cc
CFLAGS ?= -O2
WARNINGS := -Wall -Wextra -Werror -Wpedantic

.PHONY: all test clean

all: build/daybreak-blue.so

build/daybreak-blue.so: src/plugin.c
	mkdir -p build
	$(CC) $(CFLAGS) $(WARNINGS) -fPIC -shared -o $@ $<

build/abi-test: tests/abi_test.c
	mkdir -p build
	$(CC) $(CFLAGS) $(WARNINGS) -o $@ $< -ldl

test: build/daybreak-blue.so build/abi-test
	./build/abi-test ./build/daybreak-blue.so

clean:
	rm -rf build
