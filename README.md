# chipbox

[GitHub](https://github.com/mariuszskon/chipbox/)

A work-in-progress [CHIP-8](https://en.wikipedia.org/wiki/CHIP-8) interpreter/emulator, written in ANSI C and utilising the wonderful [SDL2](https://libsdl.org/) library for input, graphics and sound.

## Prebuilt binaries

[Prebuilt binaries for Windows are available here.](https://github.com/mariuszskon/chipbox/releases)

**NOTE:** This is a _command line application_ meaning double-clicking on the exe will not do what you want - you must run the application from the command line.

If you want a more up to date version, or for any other platform, you must compile it yourself (see below for details).

## Dependencies
For runtime, you need:

* [SDL2](https://libsdl.org/)

For building, you need:

* An ANSI C compiler capable of linking SDL2 (GCC 7 with make, and Visual Studio Community 2017 were tested)
* [CMake](https://cmake.org/)
* SDL2 development libraries

## Build instructions (release)
Firstly make sure you have all of the above dependencies installed! Then you can download this project, either via `git clone` or the Download ZIP option.

### Windows (Visual Studio)
1. Make sure you have CMake in your PATH. If you can run `cmake --help` from cmd without errors, you should be good. The CMake installer for Windows should offer to do this automatically.
2. Make sure you have downloaded and extracted the development libraries for SDL2 in a location you know, in this example, `X:\absolute\path\to\SDL2`
3. Open up a [Developer Command Prompt](https://docs.microsoft.com/en-us/cpp/build/building-on-the-command-line#developer-command-prompt-shortcuts) to ensure you have all of the right environmental variables set.
4. Run the following commands:

<!-- please ignore -->

    cd path\to\chipbox-master
    mkdir build && cd build
    set SDL2DIR=X:\absolute\path\to\SDL2
    cmake ..\src
    cmake --build . --config Release

5. Now you must make the SDL2.dll in the lib directory of the SDL2 development libraries accessible to the binaries. This can be done by copying it to the bin\Release directory of chipbox (where the .exe files are).

Assuming an x86 build

    copy %SDL2DIR%\lib\x86\SDL2.dll bin\Release\

Then you can run

    bin\Release\testcore

to run the tests, or

    bin\Release\chipbox_sdl "..\roms\Pong [Paul Vervalin, 1990].ch8"

to run the included Pong ROM.

### Linux

    cd path/to/chipbox-master
    mkdir build && cd build
    cmake -DCMAKE_BUILD_TYPE=Release ../src
    make

Then you can run

    bin/testcore

to run the tests, or

    bin/chipbox_sdl "../roms/Pong [Paul Vervalin, 1990].ch8"

to run the included Pong ROM.

## ROMs

For your convenience, a large pack of ROMs sourced from [mir3z/chip8-emu](https://github.com/mir3z/chip8-emu) (who in turn got it from the now defunct [chip8.com](http://chip8.com/)) has been included. The `roms.json` file provides metadata for the ROMs, such as their author, and possibly instructions for use.

You can grab just the ROMs (without chipbox) from [here](https://github.com/mariuszskon/chipbox/releases/tag/roms).
