#pragma once

class Entity {
public:
	virtual void update(float dt) = 0;
	virtual void draw() const = 0;
	virtual ~Entity() = default;
};
