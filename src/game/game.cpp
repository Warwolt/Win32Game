#include <game/game.h>

#include <engine/commands.h>
#include <engine/debug/profiling.h>
#include <engine/engine.h>
#include <engine/graphics/renderer.h>
#include <engine/input/input.h>
#include <engine/scene/scene_manager.h>
#include <engine/ui/screen_stack.h>

#include <game/scene/gameplay_scene.h>
#include <game/scene/menu_scene.h>
#include <game/ui/main_menu.h>

namespace game {

	Game initialize(engine::ScreenStack* screen_stack, engine::CommandList* commands) {
		Game game = {};

		// FIXME: We should register scenes and screens using the CommandList, I think.
		// The implementation of a lot of stuff is in CommandList::run_commands
		// So, we should NOT pass in engine internals here.

		/* Register scenes */
		commands->register_scene<MenuScene>();
		commands->register_scene<GameplayScene>();

		// scene_manager->register_scene(MenuScene::NAME, []() { return std::make_unique<MenuScene>(); });
		// scene_manager->register_scene(GameplayScene::NAME, []() { return std::make_unique<GameplayScene>(); });

		/* Register screens */
		screen_stack->register_screen(MainMenu::NAME, []() { return std::make_unique<MainMenu>(); });

		/* Load first scene */
		// scene_manager->load_scene(MenuScene::NAME);
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
