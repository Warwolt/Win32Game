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
			Continue,
			SaveGame,
			Quit,
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

			if (m_menu_index == PauseMenuItem::SaveGame) {
				commands->write_save_file("build/save_file.json");
			}

			if (m_menu_index == PauseMenuItem::Quit) {
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
		const engine::IVec2 resolution = renderer->screen_resolution();
		const engine::DrawTextOptions options = { .h_alignment = engine::HorizontalAlignment::Center };
		const int menu_width = 120;
		const int menu_height = 112;
		const engine::Rect menu_rect = {
			(resolution.x - menu_width) / 2,
			(resolution.y - menu_height) / 2,
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
		renderer->draw_text(engine::DEFAULT_FONT_ID, 16, title_rect, engine::RGBA::white(), "Paused", options);

		/* Draw menu items and cursor */
		int index = 0;
		const int item_y = menu_rect.y + 48;
		renderer->draw_text(engine::DEFAULT_FONT_ID, 16, { 0, item_y + index++ * 16, resolution.x, 0 }, engine::RGBA::white(), "Continue", options);
		renderer->draw_text(engine::DEFAULT_FONT_ID, 16, { 0, item_y + index++ * 16, resolution.x, 0 }, engine::RGBA::white(), "Save game", options);
		renderer->draw_text(engine::DEFAULT_FONT_ID, 16, { 0, item_y + index++ * 16, resolution.x, 0 }, engine::RGBA::white(), "Quit", options);
		renderer->draw_text(engine::DEFAULT_FONT_ID, 16, { menu_rect.x + 4, item_y + m_menu_index * 16 }, engine::RGBA::white(), ">");
	}

} // namespace game
