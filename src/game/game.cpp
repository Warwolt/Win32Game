#include <game/game.h>

#include <engine/commands.h>
#include <engine/debug/profiling.h>
#include <engine/engine.h>
#include <engine/graphics/renderer.h>
#include <engine/input/input.h>

namespace game {

	Game initialize(engine::Engine* /*engine*/) {
		Game game = {};
		return game;
	}

	void update(Game* /*game*/, engine::CommandList* commands, const engine::Input& input) {
		CPUProfilingScope_Game();
		if (input.keyboard.key_was_pressed_now(VK_ESCAPE)) {
			commands->quit();
		}

		if (input.keyboard.key_was_pressed_now(VK_F11)) {
			commands->toggle_fullscreen();
		}
	}

	void draw(engine::Renderer* renderer, const Game& /*game*/) {
		CPUProfilingScope_Game();
		renderer->clear_screen(engine::RGBA::black());
	}

} // namespace game
