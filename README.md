# A Platformer Game in Raylib

My submission for our CMPG121 project (2025).

## Documentation

Features are documented in [`features.md`](./features.md), program architecture is documented in [`architecture.md`](./architecture.md).

I plan on adding slideshows soon.

I will also add comments to the headers and (more selectively) source files.

## How to Compile and Run

### Windows with Code::Blocks

First, open the project ([`cmpg121_raylib_project.cbp`](./cmpg121_raylib_project.cbp)) in Code::Blocks.

Download the windows raylib library from GitHub, specifically the [raylib-5.5_win64_mingw-w64.zip](https://github.com/raysan5/raylib/releases/download/5.5/raylib-5.5_win64_mingw-w64.zip) file.

Extract the zip file into the project (so the `raylib-5.5_win64_mingw-w64` folder containing the `include` and `lib` folders must be in the same folder as the `cmpg121_raylib_project.cbp` file).

Select the "Build and Run" button (or press F9) to build and run the program. By default a Release build is created, you can also switch to a Debug build to enable some debug overlays as well as a fly key (F).

### Linux with the Terminal

On Linux, builds are done with the [nob.h](https://github.com/tsoding/nob.h) library.

First, bootstrap the build script by compiling nob with `cc -o nob nob.c`.

Now, generate the build config with `./nob init`.

You can now (optionally) edit `build/config.h` to select if you want a release or debug build, native linux or cross-compiled windows build, or wayland-native or x11-native build by commenting or uncommenting the options.

Once the build has been configured, you can build and run the game with `./nob run`. Note that to cross compile for windows, you'll have to download and extract raylib v5.5 files as explained above in the "Windows with Code::Blocks" section.

## Licensing

This library is set free under the [Unlicense](https://unlicense.org/).
This means that it is released into the public domain
for all to use as they see fit.
Use it in a way I'll approve of,
use it in a way I won't,
doesn't make much of a difference to me.

I only ask (as a request in the name of common courtesy,
**not** as a legal requirement of any sort)
that you do not claim this work as your own
but credit me as appropriate.

Also, this *is* a university project, so I'll just note here that while copying this project and claiming it as your own might not be *illegal*, it would still be plagiarism and subject to university disciplinary procedures. So don't try passing it off as your own project, because it's not.

The full terms are as follows:

```
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <https://unlicense.org/>
```
