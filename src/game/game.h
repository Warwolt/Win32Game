#pragma once

#include <engine/commands.h>

#include <optional>
#include <vector>

namespace engine {
	struct Engine;
	struct Input;
	class Renderer;
} // namespace engine

namespace game {

	struct Game {
	};

	Game initialize(engine::SceneManager* scene_manager, engine::ScreenStack* screen_stack, engine::CommandList* commands);
	void update(Game* game, const engine::Input& input, engine::CommandList* commands);
	void draw(engine::Renderer* renderer, const Game& game);

} // namespace game
