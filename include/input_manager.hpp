#pragma once

#include <functional>
#include <unordered_map>

#include "raylib.h"

/*
 * Abstraction over Raylib's keyboard input
 * Calls callbacks on key press, key release, or while key down
 */

class InputManager {
	std::unordered_map<KeyboardKey, const std::function<void()>> pressCallbacks;
	std::unordered_map<KeyboardKey, const std::function<void()>> releaseCallbacks;
	std::unordered_map<KeyboardKey, const std::function<void(float dt)>> sustainCallbacks;

	// the constructor is declared private to prevent new InputManager
	// instances from being created; other code can only obtain a
	// InputManager from the get method
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
