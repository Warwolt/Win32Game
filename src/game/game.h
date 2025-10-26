#pragma once

#include <engine/commands.h>

#include <vector>

namespace engine {
	struct Engine;
	struct InputDevices;
	class Renderer;
} // namespace engine

namespace game {

	struct Game {
	};

	Game initialize(engine::Engine* engine);
	void update(Game* game, engine::CommandList* commands, const engine::InputDevices& input);
	void draw(engine::Renderer* renderer, const Game& game);

} // namespace game
