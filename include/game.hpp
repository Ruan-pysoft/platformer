#pragma once

#include <memory>

#include "scene.hpp"

// the game class implements all the program logic;
// the main function just handles library setup and teardown as well as the
// program loop

class Game {
	std::unique_ptr<Scene> scene;
public:
	Game();

	Scene &get_scene() const;
	void set_scene(std::unique_ptr<Scene> new_scene);

	void update();
	void draw() const;
	void update_scene();
};
