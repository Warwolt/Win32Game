#pragma once

// #include <game/scene/menu_scene.h>
#include <game/ui/pause_menu.h>

#include <engine/commands.h>
#include <engine/graphics/renderer.h>
#include <engine/input/input.h>

#include <windows.h>

namespace game {

	void PauseMenu::update(const engine::Input& input, engine::CommandList* commands) {
		if (input.keyboard.key_was_pressed_now(VK_ESCAPE)) {
			commands->pop_screen();
		}
		// commands->load_scene(MenuScene::NAME);
	}

	void PauseMenu::draw(engine::Renderer* renderer) const {
		// draw a black box with the word "pause" in it
		const engine::IVec2 screen_resolution = renderer->screen_resolution();
		const int menu_width = 100;
		const int menu_height = 100;
		engine::Rect menu_rect = {
			(screen_resolution.x - menu_width) / 2,
			(screen_resolution.y - menu_height) / 2,
			menu_width,
			menu_height,
		};
		renderer->draw_rect_fill(menu_rect, engine::RGBA::black());
	}

} // namespace game
