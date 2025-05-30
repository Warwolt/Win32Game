#include <game/game.h>

#include <engine/graphics/renderer.h>
#include <engine/input/input.h>

namespace game {

	void update(GameState* game, const engine::InputDevices& input) {
		game->fish_pos.x = input.mouse.x;
		game->fish_pos.y = input.mouse.y;
		game->window_width = input.window_width;
		game->window_height = input.window_height;
	}

	void draw(engine::Renderer* renderer, const GameState& game) {
		renderer->clear_screen();

		engine::IVec2 window_center = { game.window_width / 2, game.window_height / 2 };

		// draw triangle
		{
			std::vector<engine::IVec2> vertices = {
				engine::IVec2 { 0, 0 },
				engine::IVec2 { 6, 0 },
				engine::IVec2 { 3, 3 },
			};
			renderer->draw_polygon_fill(vertices, engine::Color { 0, 255, 0 });
		}

		// draw fish polygon
		if (0) {
			int size = 30;
			std::vector<engine::IVec2> vertices = {
				game.fish_pos + size * engine::IVec2 { 2, 0 },
				game.fish_pos + size * engine::IVec2 { 0, -2 },
				game.fish_pos + size * engine::IVec2 { 3, -4 },
				game.fish_pos + size * engine::IVec2 { 8, 0 },
				game.fish_pos + size * engine::IVec2 { 8, -4 },
				game.fish_pos + size * engine::IVec2 { 4, 0 },
			};
			renderer->draw_polygon_fill(vertices, engine::Color { 0, 255, 0 });
		}
	}

} // namespace game
