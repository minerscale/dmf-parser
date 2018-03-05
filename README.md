# libdmf - A C library for parsing Deflemask (dmf) files

## [Documentation](https://minerscale.github.io/dmf-parser/)

Documentation is generated using [doxygen](http://www.stack.nl/~dimitri/doxygen/). You can find the docs here <https://minerscale.github.io/dmf-parser/>.

## Build Instructions

1. Make sure you have a C build chain and have installed doxygen
2. Set CC and AR to your C compiler and archiver (Tested only with gcc & ar and x86_64-x86_64-w64-mingw32-gcc & x86_64-w64-mingw32-ar)
3. Run make, if the docs aren't up to date they will build too.
4. If you must not have doxygen, remove all references to doxygen. Just remove the docs recipe in the makefile.
5. Now that you have `libdmf.a`, you can get to using it now!

## Usage

1. Put a copy of `libdmf.h` where you would put headers in your C project.
2. Place `libdmf.a` into a local lib directory (or a non-local one) idk why
3. Add `dmf` to LIBS in your make file OR `-ldmf` in your CFLAGS.
4. In your CFLAGS add `-L[relative or absolute path to libdmf.a]`
5. [RTFM... No seriously idk how you could *possibly* even this without Reading The Fucking Manual.](https://minerscale.github.io/dmf-parser/)
6. If you can't figure it out, you can reach me on Discord Minerscale#9319.