# Architecture

## File layout

The `build.sh` and `build-win.sh` scripts are used to build the program.

The `build` directory is used for temporary build files.

The `build-raylib.sh` script is used to build the Raylib library (for linux).

The `raylib` directory contains the raylib library.

The `levels` directory contains the level images.

The `include` directory contains the header files (`.hpp`).

The `src` directory contains the source files (`.cpp`).

## Program entry point

**Files**: `src/main.cpp`

The entry point holds the `main` function, which just initialises a game object and grabs a reference to the input manager instance.

It then loops over all the defined actions (see `inlclude/actions.hpp` and `src/actions.cpp`), registering them with the input manager.

It then handles Raylib setup, looping, and teardown: it initialises the window and audio device, sets the target fps, runs the main loop, and then closes the audio device and window.

The main loop consists of the following, in order:
 1. Tell the input manager to handle inputs
 2. Tell the game to update (physics, user interaction, etc)
 3. Tell the game to draw itself
 4. Tell the game to update the scene (advance to next scene, etc)

## Global constants / variables

**Files**: `include/globals.hpp`, `src/main.cpp`

This contains some global variables and simple functions.

```cpp
namespace global {

extern const int FPS;
extern const int WINDOW_WIDTH;
extern const int WINDOW_HEIGHT;
extern const int PPU; // pixels per unit
const float STAGE_WIDTH = WINDOW_WIDTH / (float)PPU;
const float STAGE_HEIGHT = WINDOW_HEIGHT / (float)PPU;

constexpr inline float units(float pixels) {
	return pixels / PPU;
}
constexpr inline float pixels(float units) {
	return units * PPU;
}

}
```

Initialisation of the constants are done in `src/main.cpp`

## Input Manager

**Files**: `include/actions.hpp`, `include/input_manager.hpp`, `src/actions.cpp`, `src/input_manager.cpp`

The input manager is a singleton class, which allows callbacks for three different input events:
 - pressing a key
 - releasing a key
 - holding a key

Only one callback of each type is allowed per key.

Most input events should be handled through actions, rather than the input manager. There are three types of actions: `ActionOnce` which triggers either on a key release or a key press; `ActionStartStop` which has two triggers, one on a key press and another on the same key being released; and `ActionSustain`, which calls registered callbacks every frame that the key is depressed.

Each type of action contains a list of registered callbacks, and when they are triggered, they call all registered callbacks.

Actions are defined in the `Action` namespace, as well as the mappings between keys and actions:

```cpp
namespace Action {

extern ActionSustain Jump;
extern ActionOnce DoubleJump;
extern ActionSustain Left;
extern ActionSustain Right;
extern ActionSustain Fly;

static const struct {
	KeyboardKey key;
	ActionOnce &action;
	bool on_press;
} INIT_KEYMAP_ONCE[] = {
	{ KEY_SPACE, DoubleJump, true },
	{ KEY_W, DoubleJump, true },
	{ KEY_UP, DoubleJump, true },
};

static const struct {
	KeyboardKey key;
	ActionStartStop &action;
} INIT_KEYMAP_STARTSTOP[] = {
};

static const struct {
	KeyboardKey key;
	ActionSustain &action;
} INIT_KEYMAP_SUSTAIN[] = {
	{ KEY_A, Left },
	{ KEY_LEFT, Left },
	{ KEY_D, Right },
	{ KEY_RIGHT, Right },
	{ KEY_SPACE, Jump },
	{ KEY_W, Jump },
	{ KEY_UP, Jump },
	{ KEY_F, Fly },
};

}
```

## Game

**Files**: `include/game.hpp`, `src/game.cpp`

Currently some level data is stored in `src/game.cpp` (level image file path + level spawn point), that should probably be moved into `include/level.hpp`...

The `Game` class holds a unique pointer to the current scene, as well as the level index. When the game is instantiated, it populates the scene with a new main menu scene.

It comes with functions to get the current scene and set the scene to a new scene.

Then there is a function to load a specific level (by supplied level index, loads the main menu on invalid index, and then sets the Game's level index) as well as one to load the next level (increments the game's level index then loads that level).

Then there's also a function to load the main menu.

Other than the scene loading functions, there is the update function, the draw function, and the update scene function (change the scene based on the current scene's output).

## Entities

**Files**: `include/entity.hpp`

`Entity` is an interface, with an update and draw function. Currently unused.

## Scenes

**Files**: `include/scene.hpp`

The `Scene` is an interface.

It also comes with a `SceneAction` class, which is an enum with three options: `Continue` means that the scene should continue running, `NextLevel` instructs the `Game` to load the next level, and `MainMenu` instructs the `Game` to switch to the main menu.

Scenes then have an `update` and `draw` function, as well as a `get_action` function, to instruct the `Game` if it should switch scenes.

## Main Menu

**Files**: `include/main_menu.hpp`, `src/main_menu.cpp`

The main menu just displays a title, as well as a play button. In the update function, hovering and clicking of the button is handled. The draw function draws it. The `get_action` function says if the game should load the fist level (via `NextLevel`, since on main menu load the level index of the game is set to -1).

## Levels

**Files**: `include/level.hpp`, `src/level.hpp`

Along with the levels, this file also includes tiles. Tiles can have a type, one of `Empty`, `Solid`, `Danger`, or `Goal`. The tile itself is a POD struct, with a type and colour. It comes with three constructors: No arguments → empty, invisible (air tile), colour argument → solid, with colour, colour and type arguments → as expected.

The level class has a gravity property.

It also has two constructors: one from an array of tiles, a width, a height, and a player spawn position. Another one from an image and a player spawn position.

Note that the player spawn position is in level space, which isn't the same as world space. There is a function `get_player_spawn` to get the player's spawn in world coordinates.

It the has a reset function for if the player dies, complete to advance to the next level, and exit to go to the main menu.

It also has `get_collider` and `get_tile_type` functions, which takes world coordinates and gives a `Rectangle` and `TileType`, respectively, for the corresponding tile. For `Empty` tiles and positions outside the level, the bounding box has position 0, 0 and size 0x0. The type for an out-of-bounds tile is `Empty`.

It then has an update, draw, and `get_action` function. The update function updates the player, then moves the camera after the player (about a four-unit world-space follow distance, half a second follow time).

Draw draws the player, and then draws all the tiles, inside a camera `Mode2D`.

## Player

**Files**: `include/player.hpp`, `src/player.cpp`

`Player` is a singleton class. It has an update and draw function, as well as a `get_pos` function and a `reset` function, which takes a spawn pos. The update function takes a reference to the level that called it.

The player is currently a bit of a mess, with the current static variables in the cpp file (which should probably be moved to class variables; then again, for a singleton class, does it really matter?)

```cpp
static bool try_jump = false;
static bool try_djump = false;
static bool walk_left = false;
static bool walk_right = false;
static bool walking = false;
static bool has_djumped = false;
static bool was_killed = false;
static bool achieved_goal = false;
static bool flying = false;
```

Here are a select few functions:

```cpp
void Player::reset(Vector2 pos) {
	this->pos = pos;

	try_jump = false;
	try_djump = false;
	walk_left = false;
	walk_right = false;
	walking = false;
	has_djumped = false;
	was_killed = false;
	achieved_goal = false;
	flying = false;
}

void Player::update(Level &level, float dt) {
	const bool grounded = on_ground(level);

	if (was_killed || achieved_goal) return;

	/*if (try_jump) std::cerr << "Trying to jump!" << std::endl;
	if (try_djump) std::cerr << "Trying to double jump!" << std::endl;
	if (walk_left) std::cerr << "Walking left!" << std::endl;
	if (walk_right) std::cerr << "Walking right!" << std::endl;*/

	if (try_jump && grounded) {
		vel.y = -jump_vel;
	}
	if (try_djump && !grounded && !has_djumped) {
		vel.y = -jump_vel;
		has_djumped = true;
	}
	if (walk_left) {
		walking = true;
		if (vel.x > 0) {
			vel.x -= walk_dec * dt;
		} else if (vel.x > -walk_speed) {
			vel.x -= walk_acc * dt;
			if (vel.x < -walk_speed) vel.x = -walk_speed;
		}
	}
	if (walk_right) {
		walking = true;
		if (vel.x < 0) {
			vel.x += walk_dec * dt;
		} else if (vel.x < walk_speed) {
			vel.x += walk_acc * dt;
			if (vel.x > walk_speed) vel.x = walk_speed;
		}
	}
	if (flying) {
		vel.y = std::min(vel.y, -jump_vel / 2.0f);
	}

	if (on_ground(level)) {
		has_djumped = false;

		vel.y = std::min(0.f, vel.y);

		if (!walking) {
			if (friction * dt >= std::abs(vel.x)) vel.x = 0;
			else if (vel.x > 0) vel.x -= friction * dt;
			else vel.x += friction * dt;
		}
	} else {
		if (!flying) vel.y += level.gravity * dt;
	}

	if (std::abs(vel.y) <= std::abs(vel.x)) {
		pos.x += vel.x * dt;
		resolve_collisions_x(level);

		pos.y += vel.y * dt;
		resolve_collisions_y(level);
	} else {
		pos.y += vel.y * dt;
		resolve_collisions_y(level);

		pos.x += vel.x * dt;
		resolve_collisions_x(level);
	}

	walking = false;
	try_jump = false;
	try_djump = false;
	walk_left = false;
	walk_right = false;
	flying = false;

	if (was_killed) level.reset();
	if (achieved_goal) level.complete();
}
```
