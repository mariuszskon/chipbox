CC = gcc
CFLAGS = -ansi -pedantic -Wall

CORE_DIR = core
BUILD_DIR = build

HEADERS = $(CORE_DIR)/core.h

all: testcore

%.o: */%.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ -c $<

testcore: testcore.o core.o
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$@ $^
