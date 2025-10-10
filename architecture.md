# Architecture

## File layout

`nob.c` is the build script, using files in `src_build/`.

The `levels` directory contains the level images.

The `include` directory contains the header files (`.hpp`).

The `src` directory contains the source files (`.cpp`).

If you want to know what a module does/its API, look in the `include/<module>.hpp` file.

If instead you want to know the implementation details, look in the `src/<module>.cpp` file.

## Memory/Resource Management

Throughout the whole program, memory is never explicitly allocated or deleted using `new`/`delete`. Rather, when dynamic allocations are needed, standard collection structures from the C++ STL is used (such as `std::vector`, `std::string`, or `std::unordered_map`), or memory will be allocated using a `std::unique_ptr`.

This means that all memory allocations and deallocations are handled through C++'s constructors and destructors, using the [RAII](https://en.wikipedia.org/wiki/Resource_acquisition_is_initialization) (Resource Acquisition Is Initialisation) technique, completely bypassing the need both for manual memory management as well as a garbage collector.

## Program Entry

**Files**: [`src/main.cpp`](./src/main.cpp)

Setup, teardown, and the main loop is all handled in the program's entry point, located in `src/main.cpp`.

It creates the `Game` object, registers keybindings, reads the game's config file, and initialises the Raylib library.

It then runs the program loop, which does the following, in the provided order:
 1. Updates the window width/height global variables to the current size
 2. Handles any input events
 3. Updates the game
 4. Renders the game
 5. Tells the game to change scene as necessary

After all that, it deinitialises the Raylib library.

Additionally, global constants and variables are initialised in `src/main.cpp`.

## Global Constants and Variables

**Files**: [`include/globals.hpp`](./include/globals.hpp), [`src/main.cpp`](./src/main.cpp)

Globals are all declared in `include/globals.hpp` and initialised in `src/main.cpp`.

Currently, the global values are:
 - `config` – stores the game's config. Currently just the default window width & height, and whether it should be windowed, borderless, or fullscreen.
 - `FPS` – the games' target fps
 - `WINDOW_WIDTH` and `WINDOW_HEIGHT`
 - `PPU` – the amount of pixels in an in-game unit
 - `quit` – flag to quit the game
 - `PHYSICS_FPS` – the framerate of the physics engine
 - `DATA_DIR` – the folder where game data is stored
 - `PERSONAL_BESTS_FILE` – the file in which personal bests are tracked (within the data directory)

## Game Configuration

**Files**: [`src/config.cpp`](./src/config.cpp), [`include/config.hpp`](./include/config.hpp)

## Input Handling

### Base Abstraction

**Files**: [`src/input_manager.cpp`](./src/input_manager.cpp), [`include/input_manager.hpp`](./include/input_manager.hpp)

The main interface with user input is done through the `InputManager` singleton class, which abstracts away the library's functions.

The `InputManager`, of which there only ever exists a single instance in the project, maintains three maps of callbacks associated with keyboard keys, which are managed with the `register*` and `deregister*` methods.

First is a list of keypress callbacks, where the callback associated with a given key is called when that key is pressed.

Then there is a list of release callbacks, where the callback associated with a given key is called when that key is released.

Lastly, there's a list of "sustain" callbacks, where the associated callback is called each frame while the key is down with the duration of the frame supplied as an argument.

### Higher Level API

**Files**: [`src/actions.cpp`](./src/actions.cpp), [`include/actions.hpp`](./include/actions.hpp)

The game doesn't tend to work with the `InputManager` backend, instead interacting with the higher-level `Action*` classes.

An `Action` is, conceptually, a set of callbacks which might change dynamically, which is associated with some set of keyboard keys. An action is triggered by any of its associated keys, and when it is triggered it calls all of its callbacks.

There are three types of `Action`:

 1. `ActionOnce` is an action which is triggered either on a key press or on a key release event.
 2. `ActionStartStop` is an action which is triggered both when a key is pressed and when it is released; it maintains different callbacks for the key press and key release events.
 3. `ActionSustain` is an action which is triggered as long as a key is held down.

When registering a callback, rather than having the programmer manually keep track of when it should be deregistered, instead they are provided with a `CallbackHandle` object. The `CallbackHandle` keeps track of the hashmap where the callback is stored as well as the ID by which it is known. It then deletes the callback from said hashmap (therefore deregistering it) in its destructor, allowing the programmer to use C++'s RAII abilities to manage callback deregistration.

In other words, if a callback is to be associated with a certain `Action`, but only for the lifetime of a certain object, then the `CallbackHandle` should be stored as a field of the object. Then, when the object is destroyed the `CallbackHandle`'s destructor is automatically called, removing the callbacks from the `Action`'s associated callbacks.

#### Available actions

The game's available actions are all declared in the `Action` namespace of the header file and instantiated in the source file.

The keymaps (which keys are associated with the actions) are defined in the header file, as `static const` arrays to both allow it to be compiled out if not used within a certain translation unit, as well as to not cause any linker clashes.

The keymaps are (in my opinion) quite easy to read, but here is a plain list:

 - Single Press/Release Actions:
   - Double Jump: on press of `W`, `↑`, or `K`
   - Suicide (go to last checkpoint/level spawn): on press of `X`
   - Reset (go to level spawn, reset level timer): on press of `R`
   - Pause: on press of `Esc`
   - Next Level: on press of `Enter` or `Space`
 - Press and Release Actions:
   - None so far
 - Continuous Actions:
   - Move Left: on press of `A`, `←`, or `H`
   - Move Right: on press of `D`, `→`, or `L`
   - Jump: on press of `W`, `↑`, or `K`
   - Slam: on press of `S`, `↓`, or `J`
   - Fly (only in debug builds): on press of `F`

Note how jumps and double jumps are mapped to the same keys, but jumping is a continuous action while double jumping is a single action. This means that you can hold the jump key in to jump again as soon as you land, but to double jump you have to release and press again the jump key.

## The `Game` Object

**Files**: [`src/game.cpp`](./src/game.cpp), [`include/game.hpp`](./include/game.hpp)

The `Game` object handles the game's interface. It keeps track of the current `Scene` (which deals with updating and rendering specific menus/levels/etc), and handles moving between `Scene`s.

Currently, it doesn't really do much, just serving as an abstraction layer in-between the `main` function and different `Scenes`, and handles some minimal library abstraction.

In debug builds it also keeps track of the fps over a five second period and draws a historical fps graph, which is handled through file-level global variables. This is bad practice, however it is only used as a quick debugging tool in development/debug builds of the game, and is compiled out of release builds, so I have deemed it acceptable.

When the game is told to update the game, it just calls the current `Scene`'s update function, providing the current frame time.

When it is told to draw the game, it calls the library's `BeginDrawing` and `EndDrawing` functions so that scenes need not be aware of it (which also theoretically allows nested scenes), calls the scene's draw function, and draws an fps counter in the bottom right corner of the screen.

When it is told to change scenes, it will call the scene's `post_draw` function to allow for any updates that needs to be done before scenes are changed but not before the current screen is drawn, and then it transitions to a new scene if instructed to do so by the current scene.

## The `Scene` Abstraction

**Files**: [`include/scene.hpp`](./include/scene.hpp)

The `Scene` class is an [abstract class](https://en.wikipedia.org/wiki/Abstract_type). That is, it declares some functions which child classes must implement, without implementing them itself. This means that you cannot create a `Scene` object directly, but that you must instead create a sub-class which provides implementations for the so-called "pure virtual" functions and then create an instance of that sub-class.

The purpose of the `Scene` class is to describe an API that all `Scene`s must implement, which is then used by the `Game` object.

The `Scene` defines the following methods and fields:
 - The public field `transition` is used to indicate if the next scene should be loaded. For now it only contains said next scene for the game to switch to. In the future it might specify transition effects or the like.
 - The function `update` must be provided by a `Scene` implementation, and it takes the frame time (delta time or `dt`) as its only argument.
 - The function `draw` must be provided by a `Scene` implementation.
 - The function `post_draw` may be provided by a `Scene` implementation, but by default does nothing if not provided.

### The `MainMenu` Scene

**Files**: [`src/main_menu.cpp`](./src/main_menu.cpp), [`include/main_menu.hpp`](./include/main_menu.hpp)

The `MainMenu` scene is initially loaded by the game and displays the main menu.

### The `LevelSelect` Scene

**Files**: [`src/level_select.cpp`](./src/level_select.cpp), [`include/level_select.hpp`](./include/level_select.hpp)

The `LevelSelect` scene provides buttons to play specific levels, as well as the option to do a challenge run, where you play all the levels sequentially and try to improve your overall time.

### The `LevelScene` Scene

**Files**: [`src/level_scene.cpp`](./src/level_scene.cpp), [`include/level_scene.hpp`](./include/level_scene.hpp)

### The `SingleRun` Scene

**Files**: [`src/singlerun.cpp`](./src/singlerun.cpp), [`include/singlerun.hpp`](./include/singlerun.hpp)

## The `gui` Module

**Files**: [`src/gui.cpp`](./src/gui.cpp), [`include/gui.hpp`](./include/gui.hpp)

## Level Overlays

**Files**: [`src/overlay.cpp`](./src/overlay.cpp), [`include/overlay.hpp`](./include/overlay.hpp)

## Defining and Creating Levels

**Files**: [`src/levels_list.cpp`](./src/levels_list.cpp), [`include/levels_list.hpp`](./include/levels_list.hpp), [`levels/*.png`](./levels/)

### The `Level` Object

**Files**: [`src/level.cpp`](./src/level.cpp), [`include/level.hpp`](./include/level.hpp)

### `Tile`s

**Files**: [`src/level.cpp`](./src/level.cpp), [`include/level.hpp`](./include/level.hpp)

## The Player

**Files**: [`src/player.cpp`](./src/player.cpp), [`include/player.hpp`](./include/player.hpp)

## Statistics Tracking

**Files**: [`src/stats.cpp`](./src/stats.cpp), [`include/stats.hpp`](./include/stats.hpp)

## Utilities

**Files**: [`src/util.cpp`](./src/util.cpp), [`include/util.hpp`](./include/util.hpp)
