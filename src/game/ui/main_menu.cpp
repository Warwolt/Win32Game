#include <game/ui/main_menu.h>

#include <game/game_data.h>
#include <game/scene/gameplay_scene.h>

#include <engine/commands.h>
#include <engine/debug/logging.h>
#include <engine/graphics/renderer.h>
#include <engine/input/input.h>
#include <engine/ui/debug_screen/debug_screen.h>

#include <filesystem>
#include <windows.h>

namespace game {

	struct MainMenuItem {
		enum {
			NewGame,
			Continue,
			Debug,
			Quit,
			Count,
		};
	};

	void MainMenu::update(GameData* game, const engine::Input& input, engine::CommandList* commands) {
		/* Check save file */
		m_save_file_exists = std::filesystem::exists("build/save_file.json");

		/* Update menu */
		{
			/* Menu items */
			if (input.bindings.action_was_pressed_now("ui_confirm")) {
				if (m_menu_index == MainMenuItem::NewGame) {
					*game = GameData {}; // reset game data
					commands->load_scene(GameplayScene::NAME);
				}

				if (m_menu_index == MainMenuItem::Continue && m_save_file_exists) {
					commands->load_save_file("build/save_file.json");
					commands->load_scene(GameplayScene::NAME);
				}

				if (m_menu_index == MainMenuItem::Debug) {
					commands->push_screen(engine::DebugScreen::NAME);
				}

				if (m_menu_index == MainMenuItem::Quit) {
					commands->quit();
				}
			}

			/* Menu navigation */
			if (input.keyboard.key_was_pressed_now(VK_UP)) {
				m_menu_index = (MainMenuItem::Count + m_menu_index - 1) % MainMenuItem::Count;
			}
			if (input.keyboard.key_was_pressed_now(VK_DOWN)) {
				m_menu_index = (MainMenuItem::Count + m_menu_index + 1) % MainMenuItem::Count;
			}
		}
	}

	void MainMenu::draw(const GameData& /*game*/, engine::Renderer* renderer) const {
		const engine::IVec2 resolution = renderer->screen_resolution();
		const engine::DrawTextOptions options = { .h_alignment = engine::HorizontalAlignment::Center };

		/* Title */
		renderer->draw_text(engine::DEFAULT_FONT_ID, 32, { 2, 59 + 2, resolution.x, 0 }, engine::RGBA::dark_purple(), "Main Menu", options);
		renderer->draw_text(engine::DEFAULT_FONT_ID, 32, { 1, 59 + 1, resolution.x, 0 }, engine::RGBA::dark_purple(), "Main Menu", options);
		renderer->draw_text(engine::DEFAULT_FONT_ID, 32, { 0, 59 + 0, resolution.x, 0 }, engine::RGBA::purple(), "Main Menu", options);

		/* Menu items */
		int menu_index = 1;
		const engine::RGBA continue_color = m_save_file_exists ? engine::RGBA::white() : engine::RGBA::grey();
		renderer->draw_text(engine::DEFAULT_FONT_ID, 16, { 0, 75 + 16 + menu_index++ * 16, resolution.x, 0 }, engine::RGBA::white(), "New Game", options);
		renderer->draw_text(engine::DEFAULT_FONT_ID, 16, { 0, 75 + 16 + menu_index++ * 16, resolution.x, 0 }, continue_color, "Continue", options);
		renderer->draw_text(engine::DEFAULT_FONT_ID, 16, { 0, 75 + 16 + menu_index++ * 16, resolution.x, 0 }, engine::RGBA::white(), "Debug", options);
		renderer->draw_text(engine::DEFAULT_FONT_ID, 16, { 0, 75 + 16 + menu_index++ * 16, resolution.x, 0 }, engine::RGBA::white(), "Quit", options);

		/* Cursor */
		renderer->draw_text(engine::DEFAULT_FONT_ID, 16, { 75, 75 + 32 + m_menu_index * 16 }, engine::RGBA::white(), ">");
	}

} // namespace game
