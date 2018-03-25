cmake_minimum_required(VERSION 3.10.3) # only tested with this version
project(chipbox)

set(SDL2_INCLUDE_DIR $ENV{SDL2_INCLUDE_DIR}) # where SDL.h can be found
set(SDL2_LIBRARIES $ENV{SDL2_LIBRARIES}) # SDL2 library, and SDL2main on Windows

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# display warnings
if (CMAKE_COMPILER_IS_GNUCC)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ansi -pedantic -Wall -Wextra")
endif()

# build core
set(CHIPBOX_CORE_DIR ${CMAKE_SOURCE_DIR}/core)
file(GLOB CHIPBOX_CORE_SOURCES ${CHIPBOX_CORE_DIR}/*)
add_library(chipbox_core STATIC ${CHIPBOX_CORE_SOURCES})
target_include_directories(chipbox_core PRIVATE ${CHIPBOX_CORE_DIR})

# build tests
set(CHIPBOX_TESTS_DIR ${CMAKE_SOURCE_DIR}/tests)
set(CHIPBOX_TESTS_SOURCES ${CHIPBOX_TESTS_DIR}/testcore.c)
add_executable(testcore ${CHIPBOX_TESTS_SOURCES})
target_include_directories(testcore PRIVATE ${CHIPBOX_CORE_DIR})
target_link_libraries(testcore chipbox_core)

# build chipbox_sdl
set(CHIPBOX_SDL_DIR ${CMAKE_SOURCE_DIR}/chipbox_sdl)
file(GLOB CHIPBOX_SDL_SOURCES ${CHIPBOX_SDL_DIR}/*)
add_executable(chipbox_sdl ${CHIPBOX_SDL_SOURCES})
target_include_directories(chipbox_sdl PRIVATE ${CHIPBOX_CORE_DIR} ${CHIPBOX_SDL_DIR} ${SDL2_INCLUDE_DIR})
target_link_libraries(chipbox_sdl chipbox_core ${SDL2_LIBRARIES})
