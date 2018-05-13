cmake_minimum_required(VERSION 3.10.3) # only tested with this version
project(chipbox)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/") # use our own FindSDL2.cmake

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# find SDL2
find_package(SDL2 REQUIRED)

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

# build utils
set(CHIPBOX_UTILS_DIR ${CMAKE_SOURCE_DIR}/utils)
file(GLOB CHIPBOX_UTILS_SOURCES ${CHIPBOX_UTILS_DIR}/*)
add_library(chipbox_utils STATIC ${CHIPBOX_UTILS_SOURCES})
target_include_directories(chipbox_utils PRIVATE ${CHIPBOX_UTILS_DIR})

# build disassembler - library
set(CHIPBOX_DAS_DIR ${CMAKE_SOURCE_DIR}/das)
set(CHIPBOX_DASCORE_SOURCES ${CHIPBOX_DAS_DIR}/dascore.c)
add_library(chipbox_dascore STATIC ${CHIPBOX_DASCORE_SOURCES})
target_include_directories(chipbox_dascore PRIVATE ${CHIPBOX_CORE_DIR} ${CHIPBOX_DAS_DIR})
# build disassembler - cli utility
set(CHIPBOX_DASCLI_SOURCES ${CHIPBOX_DAS_DIR}/das.c)
add_executable(chipbox_das ${CHIPBOX_DASCLI_SOURCES})
target_include_directories(chipbox_das PRIVATE ${CHIPBOX_CORE_DIR} ${CHIPBOX_DAS_DIR})
target_link_libraries(chipbox_das chipbox_dascore)

# build chipbox_sdl
set(CHIPBOX_SDL_DIR ${CMAKE_SOURCE_DIR}/chipbox_sdl)
file(GLOB CHIPBOX_SDL_SOURCES ${CHIPBOX_SDL_DIR}/*)
add_executable(chipbox_sdl ${CHIPBOX_SDL_SOURCES})
target_include_directories(chipbox_sdl PRIVATE ${CHIPBOX_CORE_DIR} ${CHIPBOX_SDL_DIR} ${CHIPBOX_UTILS_DIR} ${SDL2_INCLUDE_DIR})
target_link_libraries(chipbox_sdl chipbox_core chipbox_utils ${SDL2_LIBRARY})
