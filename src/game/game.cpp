#include <game/game.h>

#include <game/scene/gameplay_scene.h>
#include <game/scene/menu_scene.h>
#include <game/ui/main_menu.h>
#include <game/ui/pause_menu.h>

#include <engine/commands.h>
#include <engine/debug/profiling.h>
#include <engine/engine.h>
#include <engine/graphics/renderer.h>
#include <engine/input/input.h>
#include <engine/ui/debug_screen/debug_screen.h>
#include <engine/file/save_file.h>

namespace game {

	GameData initialize(engine::CommandList* commands) {
		GameData game = {};

		/* Register scenes */
		commands->register_scene<MenuScene>();
		commands->register_scene<GameplayScene>();

		/* Register screens */
		commands->register_screen<MainMenu>();
		commands->register_screen<PauseMenu>();
		commands->register_screen<engine::DebugScreen>();

		/* Load first scene */
		commands->load_scene(MenuScene::NAME);

		return game;
	}

	void update(GameData* /*game*/, const engine::Input& input, engine::CommandList* commands) {
		CPUProfilingScope_Game();

		/* Toggle fullscreen */
		if (input.keyboard.key_was_pressed_now(VK_F11)) {
			commands->toggle_fullscreen();
		}
	}

	void draw(engine::Renderer* /*renderer*/, const GameData& /*game*/) {
		// CPUProfilingScope_Game();
	}

	void on_save_file_loaded(GameData* game, const engine::SaveFile& save_file) {
		game->player_position.x = save_file.try_get<float>("player_pos_x").value_or(0.0f);
		game->player_position.y = save_file.try_get<float>("player_pos_y").value_or(0.0f);
	}

} // namespace game
