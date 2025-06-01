#include <game/game.h>

#include <engine/commands.h>
#include <engine/graphics/renderer.h>
#include <engine/input/input.h>

namespace game {

	void update(GameState* game, engine::CommandAPI* commands, const engine::InputDevices& input) {
		if (input.keyboard.key_was_pressed_now(VK_ESCAPE)) {
			commands->quit();
		}

		if (input.keyboard.key_was_pressed_now('1')) {
			commands->play_sound(game->assets.audio.cowbell);
		}
	}

	void draw(engine::Renderer* /*renderer*/, const GameState& /*game*/) {
	}

} // namespace game
