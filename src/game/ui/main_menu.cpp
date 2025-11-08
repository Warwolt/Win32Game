#include <game/ui/main_menu.h>

#include <game/scene/gameplay_scene.h>
#include <game/game_data.h>

#include <engine/commands.h>
#include <engine/debug/logging.h>
#include <engine/graphics/renderer.h>
#include <engine/input/input.h>
#include <engine/ui/debug_screen/debug_screen.h>

#include <windows.h>

namespace game {

	struct MainMenuItem {
		enum {
			NewGame,
			LoadGame,
			Debug,
			Quit,
			Count,
		};
	};

	void MainMenu::update(GameData* game, const engine::Input& input, engine::CommandList* commands) {
		if (input.keyboard.key_was_pressed_now(VK_RETURN)) {
			if (m_menu_index == MainMenuItem::NewGame) {
				*game = GameData {}; // reset game data
				commands->load_scene(GameplayScene::NAME);
			}

			if (m_menu_index == MainMenuItem::LoadGame) {
				// FIXME: add Load Game Screen that lists all save files?
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

		if (input.keyboard.key_was_pressed_now(VK_UP)) {
			m_menu_index = (MainMenuItem::Count + m_menu_index - 1) % MainMenuItem::Count;
		}
		if (input.keyboard.key_was_pressed_now(VK_DOWN)) {
			m_menu_index = (MainMenuItem::Count + m_menu_index + 1) % MainMenuItem::Count;
		}
	}

	void MainMenu::draw(const GameData& /*game*/, engine::Renderer* renderer) const {
		/* Title */
		renderer->draw_text(engine::DEFAULT_FONT_ID, 32, { 56 + 2, 59 + 2 }, engine::RGBA::dark_purple(), "Main Menu");
		renderer->draw_text(engine::DEFAULT_FONT_ID, 32, { 56 + 1, 59 + 1 }, engine::RGBA::dark_purple(), "Main Menu");
		renderer->draw_text(engine::DEFAULT_FONT_ID, 32, { 56 + 0, 59 + 0 }, engine::RGBA::purple(), "Main Menu");

		/* Menu items */
		int menu_index = 1;
		const int menu_item_x = 90;
		renderer->draw_text(engine::DEFAULT_FONT_ID, 16, { menu_item_x, 75 + 16 + menu_index++ * 16 }, engine::RGBA::white(), "New Game");
		renderer->draw_text(engine::DEFAULT_FONT_ID, 16, { menu_item_x, 75 + 16 + menu_index++ * 16 }, engine::RGBA::white(), "Load Game");
		renderer->draw_text(engine::DEFAULT_FONT_ID, 16, { menu_item_x, 75 + 16 + menu_index++ * 16 }, engine::RGBA::white(), "Debug");
		renderer->draw_text(engine::DEFAULT_FONT_ID, 16, { menu_item_x, 75 + 16 + menu_index++ * 16 }, engine::RGBA::white(), "Quit");

		/* Cursor */
		renderer->draw_text(engine::DEFAULT_FONT_ID, 16, { menu_item_x - 14, 75 + 32 + m_menu_index * 16 }, engine::RGBA::white(), ">");
	}

} // namespace game
