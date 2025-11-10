#pragma once

#include <game/scene/menu_scene.h>
#include <game/ui/pause_menu.h>

#include <engine/commands.h>
#include <engine/graphics/renderer.h>
#include <engine/input/input.h>

#include <windows.h>

namespace game {

	using namespace std::chrono_literals;

	struct PauseMenuItem {
		enum {
			Continue,
			SaveGame,
			Quit,
			Count,
		};
	};

	void PauseMenu::update(GameData* /*game*/, const engine::Input& input, engine::CommandList* commands) {
		/* Update menu */
		{
			/* Close menu */
			if (input.bindings.action_was_pressed_now("ui_close")) {
				commands->pop_screen();
			}

			/* Menu items */
			if (input.bindings.action_was_pressed_now("ui_confirm")) {
				if (m_menu_index == PauseMenuItem::Continue) {
					commands->pop_screen();
				}

				if (m_menu_index == PauseMenuItem::SaveGame) {
					commands->write_save_file("build/save_file.json");
					m_state = State::ShowSaveConfirmation;
					m_last_save = input.time_now;
				}

				if (m_menu_index == PauseMenuItem::Quit) {
					commands->load_scene(MenuScene::NAME);
				}
			}

			/* Menu navigation */
			if (input.keyboard.key_was_pressed_now(VK_UP)) {
				m_menu_index = (PauseMenuItem::Count + m_menu_index - 1) % PauseMenuItem::Count;
			}
			if (input.keyboard.key_was_pressed_now(VK_DOWN)) {
				m_menu_index = (PauseMenuItem::Count + m_menu_index + 1) % PauseMenuItem::Count;
			}
		}

		/* Update save message */
		if (m_state == State::ShowSaveConfirmation) {
			engine::Time elapsed_time = input.time_now - m_last_save;
			if (elapsed_time >= 1000ms) {
				m_state = State::ShowPauseMenu;
			}
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
		const engine::Rect title_rect = {
			menu_rect.x,
			menu_rect.y + 16,
			menu_rect.width,
			menu_rect.height,
		};

		/* Draw background */
		renderer->draw_rect_fill(menu_rect, engine::RGBA::black());

		if (m_state == State::ShowPauseMenu) {
			/* Draw title */
			renderer->draw_text(engine::DEFAULT_FONT_ID, 16, title_rect, engine::RGBA::white(), "Paused", options);

			/* Draw menu items */
			int index = 0;
			const int item_y = menu_rect.y + 48;
			renderer->draw_text(engine::DEFAULT_FONT_ID, 16, { 0, item_y + index++ * 16, resolution.x, 0 }, engine::RGBA::white(), "Continue", options);
			renderer->draw_text(engine::DEFAULT_FONT_ID, 16, { 0, item_y + index++ * 16, resolution.x, 0 }, engine::RGBA::white(), "Save game", options);
			renderer->draw_text(engine::DEFAULT_FONT_ID, 16, { 0, item_y + index++ * 16, resolution.x, 0 }, engine::RGBA::white(), "Quit", options);

			/* Draw cursor*/
			renderer->draw_text(engine::DEFAULT_FONT_ID, 16, { menu_rect.x + 4, item_y + m_menu_index * 16 }, engine::RGBA::white(), ">");
		}

		if (m_state == State::ShowSaveConfirmation) {
			/* Draw saved message */
			const engine::Rect message_rect = { menu_rect.x, menu_rect.y + (menu_rect.height - 16) / 2, menu_rect.width, menu_rect.height };
			renderer->draw_text(engine::DEFAULT_FONT_ID, 16, message_rect, engine::RGBA::yellow(), "Game saved!", options);
		}
	}

} // namespace game
