#include <game/game.h>

#include <engine/graphics/renderer.h>
#include <engine/input/input.h>

namespace game {

	void update(GameState* game, const engine::InputDevices& input) {
		game->figure_pos.x = input.window_width / 2;
		game->figure_pos.y = input.window_height / 2;
	}

	void draw(engine::Renderer* renderer, const GameState& game) {
		renderer->clear_screen();

		// draw triangle
		if (0) {
			int size = 30;
			std::vector<engine::IVec2> vertices = {
				game.figure_pos + size * engine::IVec2 { 0, 0 },
				game.figure_pos + size * engine::IVec2 { 6, 0 },
				game.figure_pos + size * engine::IVec2 { 3, 3 },
			};
			renderer->draw_polygon_fill(vertices, engine::Color { 0, 255, 0 });
		}

		// draw fish polygon
		if (1) {
			int size = 30;
			std::vector<engine::IVec2> vertices = {
				game.figure_pos + size * engine::IVec2 { 0, 0 },
				game.figure_pos + size * engine::IVec2 { -2, 1 },
				game.figure_pos + size * engine::IVec2 { -4, 0 },
				game.figure_pos + size * engine::IVec2 { -2, -1 },
				game.figure_pos + size * engine::IVec2 { 2, 1 },
				game.figure_pos + size * engine::IVec2 { 2, -1 },
			};
			renderer->draw_polygon_fill(vertices, engine::Color { 0, 255, 0, 50 });
		}

		// draw crown
		if (1) {
			int size = 30;
			std::vector<engine::IVec2> vertices = {
				game.figure_pos + size * engine::IVec2 { 0, -1 },
				game.figure_pos + size * engine::IVec2 { 1, 0 },
				game.figure_pos + size * engine::IVec2 { 2, -1 },
				game.figure_pos + size * engine::IVec2 { 2, 1 },
				game.figure_pos + size * engine::IVec2 { -2, 1 },
				game.figure_pos + size * engine::IVec2 { -2, -1 },
				game.figure_pos + size * engine::IVec2 { -1, 0 },
			};
			renderer->draw_polygon_fill(vertices, engine::Color { 0, 255, 0, 127 });
		}
	}

} // namespace game
