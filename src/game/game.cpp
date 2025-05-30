#include <game/game.h>

#include <engine/graphics/renderer.h>
#include <engine/input/input.h>

namespace game {

	void update(GameState* game, const engine::InputDevices& input) {
		if (input.mouse.left_button.is_pressed()) {
			game->fish_pos.x = input.mouse.x;
			game->fish_pos.y = input.mouse.y;
		}
		else {
			game->fish_pos.x = input.window_width / 2;
			game->fish_pos.y = input.window_height / 2;
		}
	}

	void draw(engine::Renderer* renderer, const GameState& game) {
		renderer->clear_screen();

		// draw triangle
		if (0) {
			std::vector<engine::IVec2> vertices = {
				engine::IVec2 { 0, 0 },
				engine::IVec2 { 6, 0 },
				engine::IVec2 { 3, 3 },
			};
			renderer->draw_polygon_fill(vertices, engine::Color { 0, 255, 0 });
		}

		// draw fish polygon
		if (1) {
			int size = 30;
			std::vector<engine::IVec2> vertices = {
				game.fish_pos + size * engine::IVec2 { 0, 0 },
				game.fish_pos + size * engine::IVec2 { -2, 1 },
				game.fish_pos + size * engine::IVec2 { -4, 0 },
				game.fish_pos + size * engine::IVec2 { -2, -1 },
				game.fish_pos + size * engine::IVec2 { 2, 1 },
				game.fish_pos + size * engine::IVec2 { 2, -1 },
			};
			renderer->draw_polygon_fill(vertices, engine::Color { 0, 255, 0 });
		}
	}

} // namespace game
