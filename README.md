# chipbox

[GitHub](https://github.com/mariuszskon/chipbox/)

A work-in-progress [CHIP-8](https://en.wikipedia.org/wiki/CHIP-8) interpreter/emulator, written in ANSI C and utilising the wonderful [SDL2](https://libsdl.org/) library for input, graphics and sound.

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

## Building

See `BUILD.md` in the source repository for details.

## ROMs

For your convenience, a large pack of ROMs sourced from [mir3z/chip8-emu](https://github.com/mir3z/chip8-emu) (who in turn got it from the now defunct [chip8.com](http://chip8.com/)) has been included in the source repository. The `roms.json` file provides metadata for the ROMs, such as their author, and possibly instructions for use.

You can grab just the ROMs (without chipbox) from [here](https://github.com/mariuszskon/chipbox/releases/tag/roms).

## References

Cowgod's nice, influential, but unfortunately inaccurate reference: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM

Mattmik's nice, accurate but less popular reference: http://mattmik.com/files/chip8/mastering/chip8.html

Wikipedia's information which is less detailed but uses footnotes to highlight common differences in implementations: https://en.wikipedia.org/wiki/CHIP-8#Opcode_table
