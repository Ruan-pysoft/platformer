# A Platformer Game in Raylib

A simple 2D platformer implemented in C++ with Raylib. Created as my CMPG121 semester project (2025).

## Documentation

Features are listed in [`features.md`](./features.md), program architecture is documented in [`architecture.md`](./architecture.md).

Slides and more extensive comments coming soon, in theory?

### Overview & Summary

This is a simple platformer game implemented in C++ using the [Raylib](https://www.raylib.com/) library. It will be submitted as my semester project for CMPG121.

Use WASD or arrow keys to move. [Vim](https://en.wikipedia.org/wiki/Vim_\(text_editor\)) motion keys are also available.

Use the `Escape` key to access the pause menu.

Additionally, the `R` key can be used to restart the level, and the `X` key can be used to kill the player (teleports to last spawn point). `Enter` and `Space` can be used to advance to the next level. In debug builds, the `F` key allows the player to fly; this is compiled out of release builds.

The features of the game include:
 - Deterministic physics
 - Coyote time
 - Level timers and personal best tracking (greatly increases fun and replayability)
 - Image-based level loading

The game is built around a `Game` object which manages `Scene`s such as menus or levels.

Each `Scene` type implements its own `draw` and `update` function to handle physics, display, and user interaction. Levels are loaded from image files based on pre-defined tiles, and the player interacts with the level using a fixed-timestep physics simulation. Keyboard input is handled through the `InputManager` abstraction, which communicates with various `Action*` objects.

Resource management is done entirely via RAII, with standard containers being used for memory allocations and handle types that deregister resources upon destructions. This entirely alleviates the need for manual memory management without introducing garbage collection, as well as simplifying other resource management concerns. The program loop handles inputs, physics updates/user interaction, drawing, and scene transitions in that order, and attempts to run at the game's target FPS (60).

For more information, see [`architecture.md`](./architecture.md). Additionally, you might want to work through some AI-generated docs at [deepwiki.com](https://deepwiki.com/Ruan-pysoft/platformer), which might be a bit wonky or surface-level at times, however it does come with an AI chatbot which could explain some things if you're struggling to understand something. Note however that the repo was indexed before Code::Blocks support was added, so it unfortunately won't be able to help you much with compiling with Code::Blocks.

Note that `architecture.md` was written by hand, with inputs wrt structuring/ordering taken from ChatGPT. It is not in any way influenced by any AI-generated docs (it was written before the repo was indexed by deepwiki, and ChatGPT didn't have the context to write effective or coherent docs anyways).

## How to Compile and Run

**IMPORTANT**: Either clone the repository with `--recurse-submodules` to clone the submodules as well, or run `git submodule init` and then `git submodule update` after downloading the repo.

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

### Precompiled

Precompiled builds are available from the "Releases" tab on GitHub. I recommend downloading the `game-release-v0.1.1.zip` file and extracting it to its own folder, from where you can run the game.

If you download the executable directly rather than the zip file, you'll have to also download the levels (`levels-v0.1.1.zip`) and unzip the `levels/` folder next to your executable, otherwise the game won't be able to load the levels.

## Licensing

First note that this *is* a university project, so I'll just note here that while copying this project and claiming it as your own might not be *illegal*, it would still be plagiarism and subject to university disciplinary procedures. So don't try passing it off as your own project, because it's not. That being said, here be my ramblings on copying my code:

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
