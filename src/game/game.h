#pragma once

#include <game/game_data.h>

#include <engine/commands.h>

namespace engine {
	struct Engine;
	struct Input;
	class InputBindings;
	class Renderer;
	class SaveFile;
	class SceneManager;
	class ScreenStack;
} // namespace engine

namespace game {

	// events
	void on_save_file_loaded(GameData* game, const engine::SaveFile& save_file);
	engine::SaveFile on_write_save_file(const GameData& game);

	// initialize
	std::string register_scenes(engine::SceneManager* scene_manager);
	void register_screens(engine::ScreenStack* screen_stack);
	void register_input_bindings(engine::InputBindings* input_bindings);

	// run
	void update(GameData* game, const engine::Input& input, engine::CommandList* commands);
	void draw(engine::Renderer* renderer, const GameData& game);

} // namespace game
