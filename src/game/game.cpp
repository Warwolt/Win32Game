#include <game/game.h>

#include <game/scene/gameplay_scene.h>
#include <game/scene/menu_scene.h>
#include <game/ui/main_menu.h>

#include <engine/commands.h>
#include <engine/debug/profiling.h>
#include <engine/engine.h>
#include <engine/graphics/renderer.h>
#include <engine/input/input.h>
#include <engine/ui/debug_screen/debug_screen.h>

namespace game {

	Game initialize(engine::CommandList* commands) {
		Game game = {};

		/* Register scenes */
		commands->register_scene<MenuScene>();
		commands->register_scene<GameplayScene>();

		/* Register screens */
		commands->register_screen<MainMenu>();
		commands->register_screen<engine::DebugScreen>();

		/* Load first scene */
		commands->load_scene(MenuScene::NAME);

		return game;
	}

	void update(Game* /*game*/, const engine::Input& /*input*/, engine::CommandList* /*commands*/) {
		CPUProfilingScope_Game();
	}

	void draw(engine::Renderer* renderer, const Game& /*game*/) {
		CPUProfilingScope_Game();
		renderer->clear_screen(engine::RGBA::black());
	}

} // namespace game
