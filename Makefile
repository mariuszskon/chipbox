CC = gcc
CFLAGS = -ansi -pedantic -Wall
SDL_FLAGS = -I/usr/include/SDL2 -L/usr/lib -lSDL2

CORE_DIR = core
CHIPBOX_SDL_DIR = chipbox_sdl
BUILD_DIR = bin

HEADERS = $(CORE_DIR)/core.h $(CORE_DIR)/cpu.h $(CORE_DIR)/fonts.h $(CORE_DIR)/log.h $(CHIPBOX_SDL_DIR)/chipbox_sdl.h $(CHIPBOX_SDL_DIR)/vm.h $(CHIPBOX_SDL_DIR)/render.h

all: $(BUILD_DIR)/testcore $(BUILD_DIR)/chipbox_sdl

%.o: */%.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ -c $<

$(CHIPBOX_SDL_DIR)/%.o: $(CHIPBOX_SDL_DIR)/%.c $(HEADERS)
	$(CC) $(CFLAGS) $(SDL_FLAGS) -o $@ -c $<

$(BUILD_DIR)/testcore: testcore.o core.o cpu.o fonts.o
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/chipbox_sdl: $(CHIPBOX_SDL_DIR)/chipbox_sdl.o $(CHIPBOX_SDL_DIR)/vm.o $(CHIPBOX_SDL_DIR)/render.o core.o cpu.o fonts.o
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(SDL_FLAGS) -o $@ $^

clean:
	rm *.o $(CHIPBOX_SDL_DIR)/*.o $(BUILD_DIR)/*
