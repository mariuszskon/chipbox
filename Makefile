CC = gcc
CFLAGS = -ansi -pedantic -Wall

CORE_DIR = core
BUILD_DIR = build

HEADERS = $(CORE_DIR)/core.h $(CORE_DIR)/cpu.h

all: $(BUILD_DIR)/testcore

%.o: */%.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ -c $<

$(BUILD_DIR)/testcore: testcore.o core.o cpu.o
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm *.o build/*
