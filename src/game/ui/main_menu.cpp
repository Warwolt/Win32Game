#include <game/ui/main_menu.h>

#include <game/scene/gameplay_scene.h>

#include <engine/commands.h>
#include <engine/debug/logging.h>
#include <engine/graphics/renderer.h>
#include <engine/input/input.h>
#include <engine/ui/debug_screen/debug_screen.h>

#include <windows.h>

namespace game {

	struct MainMenuItem {
		enum {
			Play = 0,
			Debug = 1,
			Quit = 2,
			Count,
		};
	};

	void MainMenu::update(const engine::Input& input, engine::CommandList* commands) {
		if (input.keyboard.key_was_pressed_now(VK_ESCAPE)) {
			commands->quit();
		}

		if (input.keyboard.key_was_pressed_now(VK_RETURN)) {
			if (m_menu_index == MainMenuItem::Play) {
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

	void MainMenu::draw(engine::Renderer* renderer) const {
		/* Title */
		renderer->draw_text(engine::DEFAULT_FONT_ID, 32, { 56 + 2, 59 + 2 }, engine::RGBA::dark_purple(), "Main Menu");
		renderer->draw_text(engine::DEFAULT_FONT_ID, 32, { 56 + 1, 59 + 1 }, engine::RGBA::dark_purple(), "Main Menu");
		renderer->draw_text(engine::DEFAULT_FONT_ID, 32, { 56 + 0, 59 + 0 }, engine::RGBA::purple(), "Main Menu");

		/* Menu items */
		renderer->draw_text(engine::DEFAULT_FONT_ID, 16, { 110, 75 + 16 + 1 * 16 }, engine::RGBA::white(), "Play");
		renderer->draw_text(engine::DEFAULT_FONT_ID, 16, { 110, 75 + 16 + 2 * 16 }, engine::RGBA::white(), "Debug");
		renderer->draw_text(engine::DEFAULT_FONT_ID, 16, { 110, 75 + 16 + 3 * 16 }, engine::RGBA::white(), "Quit");

		/* Cursor */
		renderer->draw_text(engine::DEFAULT_FONT_ID, 16, { 110 - 14, 75 + 32 + m_menu_index * 16 }, engine::RGBA::white(), ">");
	}

} // namespace game
