#include "actions.hpp"

#include <iostream>

#include "raylib.h"

#include "input_manager.hpp"

ActionOnce::ActionOnce() { }
void ActionOnce::register_key(KeyboardKey key, bool on_press) const {
	if (on_press) {
		InputManager::get().registerPress(key, [this]() { this->trigger(); });
	} else {
		InputManager::get().registerRelease(key, [this]() { this->trigger(); });
	}
}
ActionOnce::cb_handle_t ActionOnce::register_cb(ActionOnce::cb_t callback) {
	callbacks[id_counter] = callback;
	return { callbacks, id_counter++ };
}

void ActionOnce::trigger() const {
	for (auto &cb : callbacks) {
		cb.second();
	}
}

ActionStartStop::ActionStartStop() { }
void ActionStartStop::register_key(KeyboardKey key) {
	InputManager &inp_mgr = InputManager::get();

	inp_mgr.registerPress(key, [this]() { this->press(); });
	inp_mgr.registerRelease(key, [this]() { this->release(); });
}
ActionStartStop::cb_handle_t ActionStartStop::register_cb(
	ActionStartStop::cb_t on_start, ActionStartStop::cb_t on_end
) {
	callbacks[id_counter] = std::make_pair(on_start, on_end);
	return { callbacks, id_counter++ };
}

void ActionStartStop::press() {
	for (auto &cb : callbacks) {
		cb.second.first();
	}
}
void ActionStartStop::release() {
	for (auto &cb : callbacks) {
		cb.second.second();
	}
}

ActionSustain::ActionSustain() { }
void ActionSustain::register_key(KeyboardKey key) const {
	InputManager::get().registerSustain(key, [this](float dt) { this->trigger(dt); });
}
ActionSustain::cb_handle_t ActionSustain::register_cb(ActionSustain::cb_t callback) {
	callbacks[id_counter] = callback;
	return { callbacks, id_counter++ };
}

void ActionSustain::trigger(float dt) const {
	for (auto &cb : callbacks) {
		cb.second(dt);
	}
}

namespace Action {

ActionSustain Jump{};
ActionOnce DoubleJump{};
ActionSustain Left{};
ActionSustain Right{};
ActionSustain Fly{};

ActionOnce Suicide{};
ActionOnce Reset{};
ActionOnce Pause{};
ActionOnce NextLevel{};

}
