#pragma once

#include <functional>

#include "raylib.h"

class ActionOnce {
	std::vector<std::function<void()>> callbacks;
public:
	ActionOnce();
	void register_key(KeyboardKey key, bool on_press) const;
	void register_cb(const std::function<void()> callback);

	void trigger() const;
};

class ActionStartStop {
	std::vector<std::pair<std::function<void()>, std::function<void()>>> callbacks;
	bool active = false;
public:
	ActionStartStop();
	void register_key(KeyboardKey key);
	void register_cb(const std::function<void()> on_start, const std::function<void()> on_end);

	bool is_active() const { return active; }

	void press();
	void release();
};

class ActionSustain {
	std::vector<std::function<void(float dt)>> callbacks;
public:
	ActionSustain();
	void register_key(KeyboardKey key) const;
	void register_cb(const std::function<void(float dt)> callback);

	void trigger(float dt) const;
};

namespace Action {

extern ActionOnce Jump;
extern ActionSustain Left;
extern ActionSustain Right;

static const struct {
	KeyboardKey key;
	ActionOnce &action;
	bool on_press;
} INIT_KEYMAP_ONCE[] = {
	{ KEY_SPACE, Jump, true },
	{ KEY_W, Jump, true },
	{ KEY_UP, Jump, true },
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
};

}
