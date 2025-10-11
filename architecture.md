# Architecture

## File layout

`nob.c` is the build script, using files in `src_build/`.

The `levels` directory contains the level images.

The `include` directory contains the header files (`.hpp`).

The `src` directory contains the source files (`.cpp`).

If you want to know what a module does/its API, look in the `include/<module>.hpp` file.

If instead you want to know the implementation details, look in the `src/<module>.cpp` file.

## Overview & Summary

This is a simple platformer game implemented in C++ using the [Raylib](https://www.raylib.com/) library. It will be submitted as my semester project for CMPG121.

The features of the game include:
 - Deterministic physics
 - Coyote time
 - Level timers and personal best tracking (greatly increases fun and replayability)
 - Image-based level loading

The game is built around a `Game` object which manages `Scene`s such as menus or levels.

Each `Scene` type implements its own `draw` and `update` function to handle physics, display, and user interaction. Levels are loaded from image files based on pre-defined tiles, and the player interacts with the level using a fixed-timestep physics simulation. Keyboard input is handled through the `InputManager` abstraction, which communicates with various `Action*` objects.

Resource management is done entirely via RAII, with standard containers being used for memory allocations and handle types that deregister resources upon destructions. This entirely alleviates the need for manual memory management without introducing garbage collection, as well as simplifying other resource management concerns. The program loop handles inputs, physics updates/user interaction, drawing, and scene transitions in that order, and attempts to run at the game's target FPS (60).

## Memory/Resource Management

Throughout the whole program, memory is never explicitly allocated or deleted using `new`/`delete`. Rather, when dynamic allocations are needed, standard collection structures from the C++ STL is used (such as `std::vector`, `std::string`, or `std::unordered_map`), or memory will be allocated using a `std::unique_ptr`.

This means that all memory allocations and deallocations are handled through C++'s constructors and destructors, using the [RAII](https://en.wikipedia.org/wiki/Resource_acquisition_is_initialization) (Resource Acquisition Is Initialisation) technique, completely bypassing the need both for manual memory management as well as a garbage collector.

## Conditional Compilation

When compiling debug/development builds, the build system passes the `-DDEBUG` flag to the C++ compiler, which defines a symbol `DEBUG`. This flag is not passed to release builds.

This allows including code only in debug/dev builds of the game, or only in release builds using `#ifdef`/`#ifndef`/`#else`/`#endif` C Preprocessor directives.

This is used to include a historical fps tracker, render time display, player velocity displays, and player real-time hitbox display only in debug builds of the game and not in release builds. Similarly, a keybind to activate flight is also only included in debug builds.

The nice thing about conditional inclusion of code, rather than hiding functionality behind flags, is that that code does not increase the size of the executable, it is not sitting unused to be easily enabled by memory editing, and it does not slow down loops by checking a condition that evaluates to `false` each iteration.

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

Options which are retained across starts of the game are stored in the `Config` structure. Note that there currently aren't many such values.

The `Config` struct consists of fields of the types and names of the different config entries. It then supplies read and write methods to read and write config to/from a stream. It also has a `had_uninits` flag which is used to determine if a write is needed.

The functions for reading and writing the config is implemented with a lot of help from C preprocessor macros to make repetitive tasks easier and reduce code duplication.

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

In debug builds it also keeps track of the fps over a five second period and draws a historical fps graph, which is handled through file-level global variables. This implementation is not ideal, however it is only used as a quick debugging tool in development/debug builds of the game, and is compiled out of release builds, so I have deemed it acceptable.

In the `update` method it just calls the current `Scene`'s update method, providing the current frame time.

In the `draw` method it calls the library's `BeginDrawing` and `EndDrawing` functions so that scenes need not be aware of it (which also theoretically allows nested scenes), calls the scene's draw method, and draws an fps counter in the bottom right corner of the screen.

In the `update_scene` method it will call the scene's `post_draw` function to allow for any updates that needs to be done before scenes are changed but not before the current screen is drawn, and then it transitions to a new scene if instructed to do so by the current scene.

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

The `LevelScene` sits in-between a `Level` object and the `Game` object. This allows the `Level` class to focus only on handling what is needed for the level and to not worry about the `Scene` abstraction, as well as allowing levels to be used in multiple different types of scenes (see the next section on the `SingleRun` scene, which also holds a `Level` object).

The `LevelScene` essentially holds and manages a single `Level` object:
 - It can be constructed either from a `std::unique_ptr<Level>`, or from a level number, in which case it automatically loads that level
 - It provides functions to load the next level, the previous level, to reload the current level (completely resetting the level), or to exit to the main menu
   - Exiting to the main menu loads the `MainMenu` scene, switching/reloading levels does not change scenes, but just modifies the content of the current scene
 - The `update` and `draw` functions are just forwarded to the `Level` object
 - The `post_draw` function will load the next or previous level, reset the level, exit to the main menu, or do nothing, as instructed by the level
   - Note that a level just needs to signal "do nothing", "previous level", "next level", "reset level", or "main menu", and is not concerned with the mechanics of loading levels or switching scenes, allowing for clean abstraction and separation of concerns

### The `SingleRun` Scene

**Files**: [`src/singlerun.cpp`](./src/singlerun.cpp), [`include/singlerun.hpp`](./include/singlerun.hpp)

The `SingleRun` scene sits between a `Level` object and the `Game` similarly to a `LevelScene`.

However, there is some more tracking going on, as it keeps a tally of the total statistics (time played, number of deaths, etc) across levels, and disables the option to return to the previous level. Additionally, it keeps track if the player is still playing the levels, or if all the levels are completed. When this is the case, it switches modes and instead of `update`ing and `draw`ing a `Level`, it instead displays a win screen with the total statistics across all levels for that run.

At the end of such a challenge run, the player's personal best time for challenge runs is displayed and updated.

This allows the player to pursue more stamina-based personal bests, completing all levels in one go rather than retrying each individual level multiple times, pursuing an aggregate best time rather than individual best times.

## Defining and Creating Levels

**Files**: [`src/levels_list.cpp`](./src/levels_list.cpp), [`include/levels_list.hpp`](./include/levels_list.hpp), [`levels/*.png`](./levels/)

The data for the game's levels is stored in the `Levels` namespace, as well as some functions for loading levels.

Each level consists of an image file (stored here as `png`s, but in theory other formats like `bmp` should also work) which specifies the level's layout, a level spawn position (relative to the bottom left of the level, with negative y upwards, specifying the bottom left corner of the player on spawn), and a list of text objects to display above the level's tiles.

Individual levels' data is loaded into the `levels` vector, which is instantiated inside the `levels_list.cpp` file.

There is also a `make_level` function, which takes an index into the `levels` vector and returns a `std::unique_ptr` to the loaded level. If the index is invalid (too large), it returns a `nullptr`, which should in most cases cause an error message and a redirect to the main menu.

### `Tile`s

**Files**: [`src/level.cpp`](./src/level.cpp), [`include/level.hpp`](./include/level.hpp)

Each level consists of a 2D grid of `Tile`s (although note that the tiles are actually stored in a 1D `std::vector` for performance and reasons). A `Tile` is just a [POD](https://en.wikipedia.org/wiki/Passive_data_structure) struct with some constructors provided for convenience, and all the actual functionality tied to a `Tile` is implemented in the `Level` and `Player` classes.

A tile has a type, a colour, an `in_front` tag, a bounce specification, and a friction value.

There is currently five types of tiles:
 - `Empty` does not have a hitbox and does not interact with the player at all
 - `Solid` collides with the player (prevents the player from intersecting with/passing through the tile)
 - `Danger` kills the player when the player collides (intersects) with it
 - `Goal` completes the level on player collision (intersection)
 - `Checkpoint` changes the player's spawn point to its position on collision (intersection)

The colour is used to render the tile, with each tile being rendered as a 1x1 rectangle of the specified colour at the tile's level position. In theory, this can later be replaced by a texture, or some sort of union between a colour and a texture quite easily. (Though some sort of texture index would make more sense, memory-consumption wise, than actually holding a texture directly)

The `in_front` tag determines whether the tile should be drawn behind (before) or in front of (after) the player. By default, tiles should be drawn behind the player, as drawing them in front incurs a slight performance penalty. Semitransparent tiles of type `Empty` or `Checkpoint` are the only tiles likely meant to be drawn in front of the player.

The bounce specification consists of a bounciness factor for the top, bottom, and side of the tile. The bounciness factor is a float in the range 0 to 1, where 0 means a collision with that side of the tile will completely halt all movement into the tile – that is, zero the x velocity if the player is moving into the tile horizontally, or zero the y velocity if the player is moving into the tile vertically – and 1 means a collision with that side of the tile will completely reflect all movement into the tile – so if the player is moving into the tile horizontally they will then move with that same speed away from the tile, and similar for vertical motion. A bounciness factor in between zero and one will reflect only some of the player's velocity.

Note that the bounce specification only has an effect on tiles which collide with the player – currently only `Solid` tiles.

Lastly, the value specifies the player's horizontal deceleration in units per second² if the player is on top of the tile, colliding with the tile (so only on `Solid` tiles), has a horizontal velocity, and isn't holding in left or right movement keys.

A list of `Tile`s usable in levels is defined in the `Levels` namespace at the end of `level.hpp`, as well as a mapping from `Color`s to `Tile`s. This mapping is used to convert images into levels.

### The `Level` Object

**Files**: [`src/level.cpp`](./src/level.cpp), [`include/level.hpp`](./include/level.hpp)

The `Level` class is probably the most complex class in the project, only contested by the `Player` class. It handles a level's tiles, the `Player` object, a camera (`Camera2D` to ease level rendering logic), level statistics, the currently active checkpoint, and some action handles (pausing, resetting, going to the next level).

Conceptually, a level can be in one of three states:
 - `Active` – the level is currently being played
 - `Paused` – the pause screen is active, no physics updates are being processed, and a pause screen overlay is displayed
 - `WinScreen` – the level has been completed, no physics updates are being processed, and a win screen overlay is displayed

Furthermore, it can signal whether the next or previous level should be loaded, the level should be reset, or the main menu should be loaded through a `change` public field. This will then be handled by a `LevelScene` or `SingleRun`, though the `Level` class is not tied to either of these.

A level conceptually consists of a 2D array of `Tile`s and some text objects.

The `Tile`s of a level is stored in a flat `const std::vector` (which cannot be modified after the level has been constructed) along with width and height fields. The index of a `Tile` at a given `(x, y)` position in the tiles vector is easily calculated as `x + y*width` allowing efficient storage (only a single dynamic allocation of contiguous memory, rather than multiple lists of `Tiles` potentially stored in different areas of memory) and access (a single bit of fast pointer arithmetic – roughly `*(tiles + x + y*width)`, a bunch of fast arithmetic operations and a single dereference, possibly optimised down to a single assembly instruction – rather than two levels of dereferencing – roughly `*(*(tiles + x) + y)`).

The text objects are also stored in a vector, each one consisting of a `std::string` to be displayed, the `Color` it should be displayed in, and a level position where it should be displayed.

The `Level` also owns a `Player` object, but the player isn't dealt with directly other than its `update` and `draw` methods being called in the level's own, or it being fed the correct spawn location on respawns.

#### The Camera

The level and the player is rendered within the context of a camera, allowing a given tile to be rendered at a fixed world-space position and then automatically being transformed to the correct screen-space position by the camera.

The camera is roughly centered around the player, with camera movement being governed by three parameters:
 - camera play (default: four units) specifies how far from the centre of the camera the player can move in the x-axis or y-axis before the camera starts moving to centre the player again
 - camera follow (default: half a second) specifies the how fast the camera moves – when the camera is moving after the player, its velocity is such that if the player ceased moving and the camera maintained its velocity it would reach the player in the specified amount of time
 - camera minimum motion time (default: quarter of a second) specifies the minimum amount of time the camera can move for. This is so that if the player moves just outside of the allowed range, the camera smoothly moves closer to the player, rather than edging sideways for one or two frames, resulting in unpleasant, jerky camera movement when the player is moving around slowly

Note also that the camera follows the player's interpolated position rather than the players actual position. The difference between the player's interpolated and actual position is explained later.

#### Deterministic Physics System

In order to allow for consistent level complete times and consistent movement given certain input, a system of deterministic physics was implemented.

This implemented by running physics at a specific, fixed framerate determined by `global::PHYSICS_FPS` rather than whatever the framerate is the game is running at. Currently physics updates occur 32x a second, whereas the target render framerate is 60x a second.

Furthermore, the player's `update` function always operates on a fixed delta time of `1.f / global:PHYSICS_FPS` regardless of the exact time elapsed since the last time a physics update has been implemented. This prevents errors from building up by having a slightly different delta time if a physics tick took two milliseconds faster or ten milliseconds slower, which will then influence acceleration calculations, which will influence velocity calculations, which will influence position calculations.

Instead, each time a physics tick is issued, values change in a predictable and replicable fashion. In theory, this can allow the recording and replaying of inputs to recreate a playthrough, though I have not implemented this yet.

NOTE: for true deterministic physics, I should move away from floating point physics calculations and use my own fixed-point numbers instead, see [this](https://gamedev.stackexchange.com/a/174328) gamedev stackexchange answer.

However, while physics ticks are only handled at the physics framerate, the player's position is interpolated between the previous and current position based on the time elapsed since the previous physics tick for display and camera movement purposes. This allows for smooth, rather than jerky player movement, at the cost of the visual position lagging about 0.03 seconds behind the player's actual position.

#### Rendering

When drawing a level, text objects are drawn first, followed by background tiles, then the player, then foreground tiles, and lastly some UI elements (level number and level time).

If the level is paused or has been beaten, the appropriate overlay is drawn over the rendered level.

The naïve method for rendering the level's tiles would be to iterate through every tile in the level and draw it if it is a background tile, then render the player, then do the same only drawing foreground tiles.

Interestingly enough, this naïve method is enough to render smaller levels at more than a thousand fps on my (relatively old and slow) laptop, and to render even the large levels at more than 100 fps relatively consistently. As such, all additional optimisation is to give myself breathing room to improve visuals later by doing more calculations during rendering, to use less cpu (and thus decrease electricity usage), and to allow older and slower devices to play the game enjoyably, rather than out of strict necessity.

While this is still roughly what I do conceptually, there are three major optimisations I apply, which greatly speeds up rendering for larger levels.

First, I only render visible tiles. I do this by calculating the minimum and maximum x and y values that are within the camera's view, and instead of looping through _all_ x and y values of the level's tiles, I only loop between the calculated minimums and maximums.

The rendering speedup this provide is quite clear: The largest level so far is 32x256 units in size, giving a total of 8,192 tiles to render each frame. In the default window size of 800x600, only about 25x19 tiles are visible, rendering only 475 tiles (17x less!) each frame, or if I fullscreen it on my laptop for a resolution of 1920x1080 about 60x34 tiles are visible, rendering about 2040 tiles (4x less) per frame.

The second optimisation is to skip rendering for tiles which are completely invisible (alpha channel of zero). Since the majority of tiles in any given level is air tiles which are completely invisible, this skips a significant number of draw calls, noticeably improving render times.

The third optimisation is that instead of looping over all the tiles a second time to draw the foreground layer, foreground tiles are added to a vector of tiles to be drawn later during the background loop, and only this vector is looped over in the foreground loop, significantly reducing the number of items looped over a second time (typically 100's or 1000's of items to mere tens or even zero).

Additionally, by calculating the geometry and colour of the foreground tiles in the background loop already, some code duplication is not only avoided, but the foreground loop also consists only of draw calls and no logic or conditionals, which theoretically allows for great optimisation by the compiler as well as just making the loop faster as each iteration does less work.

The current implementation rarely takes more than a single millisecond to render a frame on any level and typically takes less than half a millisecond with a debug build with the game in fullscreen(!). Given that debug builds are compiled with little optimisation (default compiler optimisation level) and release builds are compiled with `-O2`, as well as the fact that debug builds are doing a bunch of extra calculations each second to display historical FPS values, the game compiled in release mode will likely struggle to *not* achieve 60 fps on most semi-modern computers with the game in windowed mode, except when doing other expensive operations like loading levels.

## The Player

**Files**: [`src/player.cpp`](./src/player.cpp), [`include/player.hpp`](./include/player.hpp)

The player keeps track of its previous and current positions, its velocity, and handles for movement actions as well as one or two other state variables.

### Player Physics

The player's `update` function should only be called on physics ticks, and will always operate on a delta time of `1.f / global::PHYSICS_FPS`.

First, the player's previous position is updated to the current position.

Next, if the player has been killed or the level has been completed, the level is signalled and the `update` function is exited.

Then, if the player is on the ground (colliding with the top of some tiles), the player's state is set as `Grounded` and the number of "coyote frames" (for implementing [coyote time](https://en.wikipedia.org/wiki/Glossary_of_video_game_terms#coyote_time)) is reset. If the player is not on the ground, its state is set to `Airborne` if the coyote time has elapsed, else the number of coyote frames left is decremented.

Next, velocity is calculated:
 - on jumps and double jumps, the player's upwards velocity is set to the jump velocity
 - if the player is "slamming" and already on the ground, the players y velocity is completely canceled and set to zero – this enables the player to voluntarily negate tile bounciness
 - if the player is moving right but inputting a left walking action, decelerate the player by the walking deceleration, and similar for motion/input in the opposite direction
 - if the player is moving left and inputting a left walking action, then accelerate the player by the walking acceleration, and cap the player's x-velocity to the player's maximum walking speed. Once again, the same is done for motion in the opposite direction
 - if the player is on the ground, then prevent the player from moving downwards
   - furthermore, if there isn't a walk left or walk right input, apply the maximum friction from tiles below the player
 - apply the level's gravitational acceleration, which is doubled if the player is "slamming"

Note that the player can double jump once after jumping, and "slamming" disables a player's double jump.

After velocity is calculated, the player's position is updated.

If the player is moving faster in the x direction, its x position is updated and x collisions resolved first, otherwise its y position is updated and y collisions resolved first. This prevents all sorts of stability issues and glitchiness that occurs when both x and y position is updated and then both x and y collisions are resolved, or when a certain axis is always updated and resolved first.

Collisions are resolved as follows:
 1. The player's collider rectangle is calculated
 2. All the points one unit apart from above and to the left to below and to the right of the player are looped through
    1. The tile and tile collider rectangle for that point is queried from the level
    2. If the tile is of type `Empty` or the collider has width and height zero, the tile is skipped
    3. Collisions between the player and the tile are calculated
    4. If the player is not intersecting with the tile, or the player's overlap with the tile in the opposite axis than the one that collisions are being resolved on is too small, the tile is skipped
    5. If the tile is solid, the player is moved to be adjacent to the tile, and the player's velocity is adjusted according to the tile's bounce factor. Otherwise, the player or level's state is altered as required by the tile the player has collided with

### Player Drawing

The player's `draw` function takes a value between 0 and 1 indicating how much there should be interpolated between the player's previous and current position. The player is then drawn at the selected in-between position as a simple rectangle.

## Statistics Tracking

**Files**: [`src/stats.cpp`](./src/stats.cpp), [`include/stats.hpp`](./include/stats.hpp)

Statistics is truct via the `Stats` [POD](https://en.wikipedia.org/wiki/Passive_data_structure) struct, which keeps track of various stats such as level complete time in physics ticks, number of double jumps, or number of deaths.

It comes with definitions for the `+` and `+=` operators to easily accumulate statistics to keep track of stats across levels (used in the challenge run), and comes with a `better_than` method which provides an ordering of statistics. This is used to determine if a personal best has been improved on.

Currently, for ordering completion time is considered first, then deaths and respawns, and lastly jumps.

The `stats` module also defines the `PBFile` class, which is used to access and update the player's personal bests as stored in the game's data folder.

## The `gui` Module

**Files**: [`src/gui.cpp`](./src/gui.cpp), [`include/gui.hpp`](./include/gui.hpp)

This module provides some basic [immediate mode](https://en.wikipedia.org/wiki/Immediate_mode_\(computer_graphics\)#Immediate_mode_GUI) abstractions for some gui elements, currently `Button`s and `Text`. It also provides the `GuiBox` type, which defines a position and size, but allows for centering an element along the x axis and/or the y axis automatically; compare to Raylib's `Rectangle` struct which doesn't allow automatic centering.

A `Button` has the following fields:
 - a callback to be called when it is clicked
 - a `GuiBox` defining the button's position and size
 - the text to display on the button, along with the font size
 - Three colours: the button's background colour when focused and unfocused, as well as the text colour
A `Button` comes with an `update` and a `draw` function. The `update` function handles user interaction.

A `Text` object comes with the following fields:
 - the text to display along with the font size
 - the position of the text
 - whether the text should be horizontally centered
 - the colour of the text
The `Text` struct comes only with a `draw` function, as it does not interact with the user (therefore an `update` function would be superfluous)

## Level Overlays

**Files**: [`src/overlay.cpp`](./src/overlay.cpp), [`include/overlay.hpp`](./include/overlay.hpp)

An `Overlay` is a collection of `Text` and `Button` objects, drawn with a semitransparent background, along with utility methods to add and access the `Text` and `Button` elements.

It was added as a more convenient way to handle level overlays than polluting the level class with a bunch of `Text` and `Button` fields.

## Utilities

**Files**: [`src/util.cpp`](./src/util.cpp), [`include/util.hpp`](./include/util.hpp)

### Collision Detection

The `collide` function preforms collision between two rectangular bounding boxes. Its result is in the form of a `Collision` struct.

The collision struct contains the following fields:
 - `dist` is the signed distance that the objects overlap; if it is added to the first rectangle's position, the two rectangles will no longer overlap
 - `new_pos` contains the position that the first rectangle would have to be moved to to avoid overlap. In theory this is unneeded in light of `dist`, but using `new_pos` rather than `dist` removes floating-point error
 - `x_touches` and `y_touches` indicates whether the two bounding boxes are touching/overlapping on the x and y axes, respectively

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
