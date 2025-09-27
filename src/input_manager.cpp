#include "input_manager.hpp"

#include "raylib.h"

InputManager::InputManager()
: pressCallbacks{}, releaseCallbacks{}, sustainCallbacks{} { }

InputManager& InputManager::get() {
	static InputManager instance{};

	return instance;
}

void InputManager::handleInputs() const {
	for (auto &item : pressCallbacks) {
		if (IsKeyPressed(item.first)) item.second();
	}
	for (auto &item : releaseCallbacks) {
		if (IsKeyReleased(item.first)) item.second();
	}
	const float dt = GetFrameTime();
	for (auto &item : sustainCallbacks) {
		if (IsKeyDown(item.first)) item.second(dt);
	}
}

void InputManager::registerPress(KeyboardKey key, const std::function<void()> callback) {
	pressCallbacks.insert(std::make_pair(key, callback));
}
void InputManager::registerRelease(KeyboardKey key, const std::function<void()> callback) {
	releaseCallbacks.insert(std::make_pair(key, callback));
}
void InputManager::deregisterPress(KeyboardKey key) {
	pressCallbacks.erase(key);
}
void InputManager::deregisterRelease(KeyboardKey key) {
	releaseCallbacks.erase(key);
}

void InputManager::registerSustain(KeyboardKey key, const std::function<void(float dt)> callback) {
	sustainCallbacks.insert(std::make_pair(key, callback));
}
void InputManager::deregisterSustain(KeyboardKey key) {
	sustainCallbacks.erase(key);
}
