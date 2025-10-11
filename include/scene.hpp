#pragma once

#include <memory>

/*
 * Defines the basic interface used by different scenes
 * No implementation is provided, only an API
 */

class Scene;
struct SceneTransition {
	std::unique_ptr<Scene> next;
};

class Scene {
public:
	SceneTransition transition = { nullptr };

	virtual ~Scene() = default;

	virtual void update(float dt) = 0;
	virtual void draw() const = 0;
	virtual void post_draw() {};
};
