#include <game/game.h>

#include <engine/graphics/renderer.h>
#include <engine/input/input.h>

namespace game {

	void update(GameState* game, const engine::InputDevices& input) {
		game->window_size = { input.window_width, input.window_height };
		game->show_renderer_test_screen = true;
	}

	void draw(engine::Renderer* renderer, const GameState& game) {
		renderer->clear_screen();
		if (game.show_renderer_test_screen) {
			engine::draw_renderer_test_screen(renderer, game.window_size);
		}
	}

} // namespace game
