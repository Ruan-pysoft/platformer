#pragma once

#include "raylib.h"

enum class SceneAction { Continue, NextLevel, MainMenu };

class Scene {
public:
	virtual ~Scene() = default;

	virtual void update(float dt) = 0;
	virtual void draw() const = 0;
	virtual SceneAction get_action() const = 0;
};
