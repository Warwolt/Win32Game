#pragma once

#include <game/game_data.h>

#include <engine/commands.h>

namespace engine {
	struct Engine;
	struct Input;
	class Renderer;
	class SaveFile;
	class SceneManager;
} // namespace engine

namespace game {

	void on_save_file_loaded(GameData* game, const engine::SaveFile& save_file);
	engine::SaveFile on_write_save_file(const GameData& game);

	std::string register_scenes(engine::SceneManager* scene_manager);
	GameData initialize(engine::CommandList* commands); // FIXME: get rid of CommandList here completely
	void update(GameData* game, const engine::Input& input, engine::CommandList* commands);
	void draw(engine::Renderer* renderer, const GameData& game);

} // namespace game
