# chipbox

[GitHub](https://github.com/mariuszskon/chipbox/)

A work-in-progress [CHIP-8](https://en.wikipedia.org/wiki/CHIP-8) interpreter/emulator, written in Standard C (also known as ANSI C, ISO C, C89, C90) and utilising the wonderful [SDL2](https://libsdl.org/) library for input, graphics and sound.

## Features

* A CHIP-8 interpreter implementing all documented instructions
* Minimalist SDL2 application with visual, audio and input support on many platforms
* Options for switching between different popular interpretations of instructions ("quirks")
* Options for graphics scaling, clock speed, error logging etc.
* Simple disassembler
* Reusable pure C API

## Prebuilt binaries

You should use these if you can.

[Prebuilt binaries for Windows are available here.](https://github.com/mariuszskon/chipbox/releases)

**NOTE:** This is a _command line application_ meaning double-clicking on the exe will not do what you want - you must run the application from the command line.

If you want a more up to date version, or for any other platform, you must compile it yourself (see `BUILD.md` in the source repository).

## Quick start

chipbox can be simply called in the form `chipbox_sdl rompath` from the command line to use default settings.

The quick start instructions assume you have your command prompt/emulator/terminal open in a directory with a `bin` directory a `roms` directory. The `bin` directory has `chipbox_sdl` and the `roms` directory has `Pong [Paul Vervalin, 1990].ch8`. If you are using the prebuilt binaries and rom pack, make sure you adjust the paths based on where you extracted those.

On Windows:

    bin\chipbox_sdl "roms\Pong [Paul Vervalin, 1990].ch8"

On Linux/Unix-like systems:

    bin/chipbox_sdl "roms/Pong [Paul Vervalin, 1990].ch8"

## Input

The original CHIP-8 keypad looked like this:

    1 2 3 C
    4 5 6 D
    7 8 9 E
    A 0 B F

In chipbox, the keys are mapped to the [scancodes](https://wiki.libsdl.org/SDL_Scancode#Remarks) of the following keys:

    1 2 3 4
    Q W E R
    A S D F
    Z X C V

## Options

    Usage: chipbox_sdl [options...] [file]

    Options:
     -h, --help           show this help message
     -s, --scale          set scaling of display
     -t, --tps            set CHIP-8 ticks/instructions per second
     -m, --mode           set 'mattmik' or 'cowgod' compatibility mode
     -l, --log            set logging level (none, error, warn (default), info)

## Building

See `BUILD.md` in the source repository for details.

## ROMs

For your convenience, a large pack of ROMs sourced from [mir3z/chip8-emu](https://github.com/mir3z/chip8-emu) (who in turn got it from the now defunct [chip8.com](http://chip8.com/)) has been included in the source repository. The `roms.json` file provides metadata for the ROMs, such as their author, and possibly instructions for use.

You can grab just the ROMs (without chipbox) from [here](https://github.com/mariuszskon/chipbox/releases/tag/roms).

## References

Cowgod's nice, influential, but unfortunately inaccurate reference: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM

Mattmik's nice, accurate but less popular reference: http://mattmik.com/files/chip8/mastering/chip8.html

Wikipedia's information which is less detailed but uses footnotes to highlight common differences in implementations: https://en.wikipedia.org/wiki/CHIP-8#Opcode_table
