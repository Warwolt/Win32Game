#include <game/game.h>

#include <engine/graphics/renderer.h>
#include <engine/input/input.h>

namespace game {

	void update(GameState* game, const engine::InputDevices& input) {
		game->mouse_x = input.mouse.x;
		game->mouse_y = input.mouse.y;
		game->window_width = input.window_width;
		game->window_height = input.window_height;
	}

	void draw(engine::Renderer* renderer, const GameState& game) {
		renderer->clear_screen();

		// draw line
		if (0) {
			engine::IVec2 line_start = { game.window_width / 2, game.window_height / 2 };
			engine::IVec2 line_end = { game.mouse_x, game.mouse_y };
			renderer->draw_line(line_start, line_end, engine::Color { 0, 255, 0 });
		}

		// draw polygon
		{
			int size = 10;
			engine::IVec2 window_center = { game.window_width / 2, game.window_height / 2 };
			engine::IVec2 top = window_center + engine::IVec2 { 0, -size };
			engine::IVec2 left = window_center + engine::IVec2 { (int)std::round(-size * 0.866), (int)std::round(size * 0.5) };
			engine::IVec2 right = window_center + engine::IVec2 { (int)std::round(-size * -0.866), (int)std::round(size * 0.5) };
			renderer->draw_polygon({ { top, left, right } }, engine::Color { 0, 255, 0 });
		}
	}

} // namespace game
