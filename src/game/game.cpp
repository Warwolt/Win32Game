#include <game/game.h>

#include <engine/commands.h>
#include <engine/debug/profiling.h>
#include <engine/engine.h>
#include <engine/graphics/renderer.h>
#include <engine/input/input.h>

#include <string>

namespace game {

	Game initialize(engine::Engine* engine) {
		Game game = {};
		return game;
	}

	void update(Game* game, std::vector<engine::Command>* commands, const engine::InputDevices& input) {
		CPUProfilingScope_Game();
		if (input.keyboard.key_was_pressed_now(VK_ESCAPE)) {
			commands->push_back(engine::AppCommand_Quit());
		}

		if (input.keyboard.key_was_pressed_now(VK_F11)) {
			commands->push_back(engine::AppCommand_ToggleFullscreen());
		}
	}

	void draw(engine::Renderer* renderer, const Game& /*game*/) {
		CPUProfilingScope_Game();
		renderer->clear_screen(engine::RGBA::black());
	}

} // namespace game
