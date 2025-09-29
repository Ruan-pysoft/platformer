#pragma once

#include <memory>

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
};
