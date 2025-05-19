#include <game/game.h>

#include <engine/graphics/renderer.h>
#include <engine/input/input.h>

namespace game {

	void update(GameState* game, const engine::InputDevices& input) {
		game->line_start = { input.window_width / 2, input.window_height / 2 };
		game->line_end = engine::IVec2 { input.mouse.x, input.mouse.y };
	}

	void draw(engine::Renderer* renderer, const GameState& game) {
		renderer->clear_screen();
		renderer->draw_line(game.line_start, game.line_end, engine::Color { 0, 255, 0 });
	}

} // namespace game
