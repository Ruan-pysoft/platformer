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
void ActionOnce::register_cb(const std::function<void()> callback) {
	callbacks.push_back(callback);
}

void ActionOnce::trigger() const {
	for (auto &cb : callbacks) {
		cb();
	}
}

ActionStartStop::ActionStartStop() { }
void ActionStartStop::register_key(KeyboardKey key) {
	InputManager &inp_mgr = InputManager::get();

	inp_mgr.registerPress(key, [this]() { this->press(); });
	inp_mgr.registerRelease(key, [this]() { this->release(); });
}
void ActionStartStop::register_cb(const std::function<void()> on_start, const std::function<void()> on_end) {
	callbacks.push_back(std::make_pair(on_start, on_end));
}

void ActionStartStop::press() {
	for (auto &cb : callbacks) {
		cb.first();
	}
}
void ActionStartStop::release() {
	for (auto &cb : callbacks) {
		cb.second();
	}
}

ActionSustain::ActionSustain() { }
void ActionSustain::register_key(KeyboardKey key) const {
	InputManager::get().registerSustain(key, [this](float dt) { this->trigger(dt); });
}
void ActionSustain::register_cb(const std::function<void(float dt)> callback) {
	callbacks.push_back(callback);
}

void ActionSustain::trigger(float dt) const {
	for (auto &cb : callbacks) {
		cb(dt);
	}
}

namespace Action {

ActionOnce Jump{};
ActionSustain Left{};
ActionSustain Right{};

}
