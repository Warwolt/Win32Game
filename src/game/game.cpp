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
		engine::IVec2 line_start = { game.window_width / 2, game.window_height / 2 };
		engine::IVec2 line_end = { game.mouse_x, game.mouse_y };

		renderer->clear_screen();
		renderer->draw_rect_fill(
			engine::Rect {
				.x = game.window_width / 2,
				.y = game.window_height / 2,
				.width = 100,
				.height = 100,
			},
			engine::Color { 255, 255, 255 }
		);
		renderer->draw_line(line_start, line_end, engine::Color { 0, 255, 0 });
	}

} // namespace game
