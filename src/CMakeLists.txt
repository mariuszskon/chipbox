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
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c89 -pedantic -Wall -Wextra")
endif()

# build core
set(CHIPBOX_CORE_DIR ${CMAKE_SOURCE_DIR}/core)
file(GLOB CHIPBOX_CORE_SOURCES ${CHIPBOX_CORE_DIR}/*)
add_library(chipbox_core STATIC ${CHIPBOX_CORE_SOURCES})
target_include_directories(chipbox_core PRIVATE ${CHIPBOX_CORE_DIR})

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

# build tests - testhelpers (common utility functions)
set(CHIPBOX_TESTS_DIR ${CMAKE_SOURCE_DIR}/tests)
set(CHIPBOX_TESTHELPERS_SOURCES ${CHIPBOX_TESTS_DIR}/testhelpers.c)
add_library(testhelpers STATIC ${CHIPBOX_TESTHELPERS_SOURCES})
target_include_directories(testhelpers PRIVATE ${CHIPBOX_CORE_DIR} ${CHIPBOX_TEST_DIR})
# build tests - core
set(CHIPBOX_TESTCORE_SOURCES ${CHIPBOX_TESTS_DIR}/testcore.c)
add_executable(testcore ${CHIPBOX_TESTCORE_SOURCES})
target_include_directories(testcore PRIVATE ${CHIPBOX_CORE_DIR} ${CHIPBOX_TEST_DIR})
target_link_libraries(testcore chipbox_core testhelpers)
# build tests - das
set(CHIPBOX_TESTDAS_SOURCES ${CHIPBOX_TESTS_DIR}/testdas.c)
add_executable(testdas ${CHIPBOX_TESTDAS_SOURCES})
target_include_directories(testdas PRIVATE ${CHIPBOX_CORE_DIR} ${CHIPBOX_TEST_DIR} ${CHIPBOX_DAS_DIR})
target_link_libraries(testdas chipbox_core testhelpers chipbox_dascore)

# build utils
set(CHIPBOX_UTILS_DIR ${CMAKE_SOURCE_DIR}/utils)
file(GLOB CHIPBOX_UTILS_SOURCES ${CHIPBOX_UTILS_DIR}/*)
add_library(chipbox_utils STATIC ${CHIPBOX_UTILS_SOURCES})
target_include_directories(chipbox_utils PRIVATE ${CHIPBOX_UTILS_DIR})

# build chipbox_sdl
set(CHIPBOX_SDL_DIR ${CMAKE_SOURCE_DIR}/chipbox_sdl)
file(GLOB CHIPBOX_SDL_SOURCES ${CHIPBOX_SDL_DIR}/*)
add_executable(chipbox_sdl ${CHIPBOX_SDL_SOURCES})
target_include_directories(chipbox_sdl PRIVATE ${CHIPBOX_CORE_DIR} ${CHIPBOX_SDL_DIR} ${CHIPBOX_UTILS_DIR} ${CHIPBOX_DAS_DIR} ${SDL2_INCLUDE_DIR})
target_link_libraries(chipbox_sdl chipbox_core chipbox_utils chipbox_dascore ${SDL2_LIBRARY})
