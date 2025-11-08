#pragma once

#include <game/game_data.h>

#include <engine/commands.h>

namespace engine {
	struct Engine;
	struct Input;
	class Renderer;
} // namespace engine

namespace game {

	GameData initialize(engine::CommandList* commands);
	void update(GameData* game, const engine::Input& input, engine::CommandList* commands);
	void draw(engine::Renderer* renderer, const GameData& game);

} // namespace game
