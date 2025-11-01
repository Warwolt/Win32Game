#include <game/game.h>

#include <engine/commands.h>
#include <engine/debug/profiling.h>
#include <engine/engine.h>
#include <engine/graphics/renderer.h>
#include <engine/input/input.h>
#include <engine/scene/scene_manager.h>
#include <engine/ui/screen_stack.h>

#include <game/scene/menu_scene.h>
#include <game/scene/gameplay_scene.h>

namespace game {

	Game initialize(engine::SceneManager* scene_manager, engine::ScreenStack* screen_stack, engine::CommandList* commands) {
		Game game = {};

		scene_manager->register_scene<MenuScene>();
		scene_manager->register_scene<GameplayScene>();
		scene_manager->load_scene(MenuScene::NAME);

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
