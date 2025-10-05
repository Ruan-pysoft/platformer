#pragma once

#include <cstdint>
#include <functional>
#include <unordered_map>

#include "raylib.h"
template<typename T>
using callback_map_t = std::unordered_map<uint32_t, T>;

template<typename T>
class CallbackHandle {
	callback_map_t<T> *map;
	uint32_t id;
public:
	CallbackHandle() : map(nullptr), id(0) { }
	CallbackHandle(callback_map_t<T> &map, uint32_t id) : map(&map), id(id) { }
	~CallbackHandle() {
		detach();
	}

	CallbackHandle(const CallbackHandle&) = delete;
	CallbackHandle &operator=(const CallbackHandle&) = delete;

	CallbackHandle(CallbackHandle &&other) noexcept : map(other.map), id(other.id) {
		other.map = nullptr;
	}
	CallbackHandle &operator=(CallbackHandle &&other) noexcept {
		if (&other != this) {
			detach();
			id = other.id;
			map = other.map;
			other.map = nullptr;
		}
		return *this;
	}

	void detach() {
		if (map) map->erase(id);
		map = nullptr;
		id = 0;
	}
};

class ActionOnce {
public:
	using cb_t = std::function<void()>;
	using cb_handle_t = CallbackHandle<cb_t>;

private:
	callback_map_t<cb_t> callbacks;
	uint32_t id_counter = 0;
public:
	ActionOnce();
	void register_key(KeyboardKey key, bool on_press) const;
	cb_handle_t register_cb(cb_t callback);

	void trigger() const;
};

class ActionStartStop {
public:
	using cb_t = std::function<void()>;
	using dbl_cb_t = std::pair<cb_t, cb_t>;
	using cb_handle_t = CallbackHandle<dbl_cb_t>;

private:
	callback_map_t<dbl_cb_t> callbacks;
	uint32_t id_counter = 0;
	bool active = false;
public:
	ActionStartStop();
	void register_key(KeyboardKey key);
	cb_handle_t register_cb(cb_t on_start, cb_t on_end);

	bool is_active() const { return active; }

	void press();
	void release();
};

class ActionSustain {
public:
	using cb_t = std::function<void(float)>;
	using cb_handle_t = CallbackHandle<cb_t>;

private:
	callback_map_t<cb_t> callbacks;
	uint32_t id_counter = 0;
public:
	ActionSustain();
	void register_key(KeyboardKey key) const;
	cb_handle_t register_cb(cb_t callback);

	void trigger(float dt) const;
};

namespace Action {

extern ActionSustain Jump;
extern ActionOnce DoubleJump;
extern ActionSustain Slam;
extern ActionSustain Left;
extern ActionSustain Right;
extern ActionSustain Fly;

extern ActionOnce Suicide;
extern ActionOnce Reset;
extern ActionOnce Pause;
extern ActionOnce NextLevel;

static const struct {
	KeyboardKey key;
	ActionOnce &action;
	bool on_press;
} INIT_KEYMAP_ONCE[] = {
	{ KEY_W, DoubleJump, true },
	{ KEY_UP, DoubleJump, true },
	{ KEY_K, DoubleJump, true },
	{ KEY_X, Suicide, true },
	{ KEY_R, Reset, true },
	{ KEY_ESCAPE, Pause, true },
	{ KEY_ENTER, NextLevel, true },
	{ KEY_SPACE, NextLevel, true },
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
	{ KEY_H, Left },
	{ KEY_D, Right },
	{ KEY_RIGHT, Right },
	{ KEY_L, Right },
	{ KEY_W, Jump },
	{ KEY_UP, Jump },
	{ KEY_K, Jump },
	{ KEY_S, Slam },
	{ KEY_DOWN, Slam },
	{ KEY_J, Slam },
	{ KEY_F, Fly },
};

}
