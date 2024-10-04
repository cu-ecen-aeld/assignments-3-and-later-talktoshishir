#Makefiel for assignment 2

all:writer

CXX = gcc
ifeq ($(CROSS_COMPILE),aarch64-none-linux-gnu-)
	CXX = aarch64-none-linux-gnu-gcc
endif
writer:writer.c
	$(CXX) $^ -o $@
clean:
	rm -rf writer *.o
