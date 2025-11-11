#include <game/game.h>

#include <game/scene/gameplay_scene.h>
#include <game/scene/menu_scene.h>
#include <game/ui/main_menu.h>
#include <game/ui/pause_menu.h>
#include <game/ui/debug_screen/debug_screen.h>

#include <engine/commands.h>
#include <engine/debug/profiling.h>
#include <engine/engine.h>
#include <engine/file/save_file.h>
#include <engine/graphics/renderer.h>
#include <engine/input/input.h>
#include <engine/scene/scene_manager.h>

namespace game {

	std::string register_scenes(engine::SceneManager* scene_manager) {
		scene_manager->register_scene<MenuScene>();
		scene_manager->register_scene<GameplayScene>();
		return MenuScene::NAME;
	}

	void register_screens(engine::ScreenStack* screen_stack) {
		screen_stack->register_screen<MainMenu>();
		screen_stack->register_screen<PauseMenu>();
		screen_stack->register_screen<DebugScreen>();
	}

	void register_input_bindings(engine::InputBindings* input_bindings) {
		input_bindings->add_keyboard_binding("ui_confirm", { VK_RETURN, 'Z' });
		input_bindings->add_keyboard_binding("ui_close", { VK_ESCAPE });
		input_bindings->add_keyboard_binding("show_pause", { VK_ESCAPE });
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
		game->player_direction = save_file.try_get<Direction>("player_direction").value_or(Direction::Down);
	}

	engine::SaveFile on_write_save_file(const GameData& game) {
		engine::SaveFile save_file;
		save_file["player_pos_x"] = game.player_position.x;
		save_file["player_pos_y"] = game.player_position.y;
		save_file["player_direction"] = (int)game.player_direction;
		return save_file;
	}

} // namespace game
