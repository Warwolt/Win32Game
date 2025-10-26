#pragma once

#include <engine/commands.h>

#include <vector>

namespace engine {
	struct Engine;
	struct Input;
	class Renderer;
} // namespace engine

namespace game {

	struct Game {
	};

	Game initialize(engine::Engine* engine);
	void update(Game* game, engine::CommandList* commands, const engine::Input& input);
	void draw(engine::Renderer* renderer, const Game& game);

} // namespace game
