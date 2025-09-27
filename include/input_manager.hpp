#pragma once

#include <functional>
#include <unordered_map>

#include "raylib.h"

class InputManager {
	std::unordered_map<KeyboardKey, const std::function<void()>> pressCallbacks;
	std::unordered_map<KeyboardKey, const std::function<void()>> releaseCallbacks;
	std::unordered_map<KeyboardKey, const std::function<void(float dt)>> sustainCallbacks;

	InputManager();
public:
	static InputManager& get();

	void handleInputs() const;

	void registerPress(KeyboardKey key, const std::function<void()> callback);
	void registerRelease(KeyboardKey key, const std::function<void()> callback);
	void deregisterPress(KeyboardKey key);
	void deregisterRelease(KeyboardKey key);

	void registerSustain(KeyboardKey key, const std::function<void(float dt)> callback);
	void deregisterSustain(KeyboardKey key);
};
