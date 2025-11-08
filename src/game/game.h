#pragma once

#include <game/game_data.h>

#include <engine/commands.h>

namespace engine {
	struct Engine;
	struct Input;
	class Renderer;
	class SaveFile;
} // namespace engine

namespace game {

	void on_save_file_loaded(GameData* game, const engine::SaveFile& save_file);
	engine::SaveFile on_write_save_file(const GameData& game);

	GameData initialize(engine::CommandList* commands);
	void update(GameData* game, const engine::Input& input, engine::CommandList* commands);
	void draw(engine::Renderer* renderer, const GameData& game);

} // namespace game
