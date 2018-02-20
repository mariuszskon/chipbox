CC = gcc
CFLAGS = -ansi -pedantic -Wall

CORE_DIR = core
BUILD_DIR = bin

HEADERS = $(CORE_DIR)/core.h $(CORE_DIR)/cpu.h $(CORE_DIR)/fonts.h

all: $(BUILD_DIR)/testcore

%.o: */%.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ -c $<

$(BUILD_DIR)/testcore: testcore.o core.o cpu.o fonts.o
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm *.o $(BUILD_DIR)/*
