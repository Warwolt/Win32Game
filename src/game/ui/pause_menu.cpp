#pragma once

#include <game/scene/menu_scene.h>
#include <game/ui/pause_menu.h>

#include <engine/commands.h>
#include <engine/graphics/renderer.h>
#include <engine/input/input.h>

#include <windows.h>

namespace game {

	struct PauseMenuItem {
		enum {
			Continue = 0,
			SaveAndQuit = 1,
			Count,
		};
	};

	void PauseMenu::update(GameData* /*game*/, const engine::Input& input, engine::CommandList* commands) {
		if (input.keyboard.key_was_pressed_now(VK_ESCAPE)) {
			commands->pop_screen();
		}

		if (input.keyboard.key_was_pressed_now(VK_RETURN)) {
			if (m_menu_index == PauseMenuItem::Continue) {
				commands->pop_screen();
			}

			if (m_menu_index == PauseMenuItem::SaveAndQuit) {
				commands->load_scene(MenuScene::NAME);
			}
		}

		if (input.keyboard.key_was_pressed_now(VK_UP)) {
			m_menu_index = (PauseMenuItem::Count + m_menu_index - 1) % PauseMenuItem::Count;
		}
		if (input.keyboard.key_was_pressed_now(VK_DOWN)) {
			m_menu_index = (PauseMenuItem::Count + m_menu_index + 1) % PauseMenuItem::Count;
		}
	}

	void PauseMenu::draw(const GameData& /*game*/, engine::Renderer* renderer) const {
		const engine::IVec2 screen_resolution = renderer->screen_resolution();
		const int menu_width = 120;
		const int menu_height = 100;
		const engine::Rect menu_rect = {
			(screen_resolution.x - menu_width) / 2,
			(screen_resolution.y - menu_height) / 2,
			menu_width,
			menu_height,
		};
		const int title_width = 6 * 8;
		const engine::Rect title_rect = {
			menu_rect.x + (menu_width - title_width) / 2,
			menu_rect.y + 16
		};

		/* Draw background + title */
		renderer->draw_rect_fill(menu_rect, engine::RGBA::black());
		renderer->draw_text(engine::DEFAULT_FONT_ID, 16, title_rect, engine::RGBA::white(), "Paused");

		/* Draw menu items and cursor */
		int index = 0;
		const int item_y = menu_rect.y + 48;
		const int item_x = menu_rect.x + 20;
		renderer->draw_text(engine::DEFAULT_FONT_ID, 16, { item_x, item_y + index++ * 16 }, engine::RGBA::white(), "Continue");
		renderer->draw_text(engine::DEFAULT_FONT_ID, 16, { item_x, item_y + index++ * 16 } /*  */, engine::RGBA::white(), "Save & Quit");
		renderer->draw_text(engine::DEFAULT_FONT_ID, 16, { item_x - 16, item_y + m_menu_index * 16 }, engine::RGBA::white(), ">");
	}

} // namespace game
