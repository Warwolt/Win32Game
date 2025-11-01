#include <game/ui/main_menu.h>

#include <game/scene/gameplay_scene.h>

#include <engine/commands.h>
#include <engine/debug/logging.h>
#include <engine/graphics/renderer.h>
#include <engine/input/input.h>

#include <windows.h>

namespace game {

	void MainMenu::initialize(engine::ResourceManager* /*resources*/, engine::CommandList* /*commands*/) {
	}

	void MainMenu::update(const engine::Input& input, engine::CommandList* commands) {
		if (input.keyboard.key_was_pressed_now(VK_ESCAPE)) {
			commands->quit();
		}

		if (input.keyboard.key_was_pressed_now(VK_RETURN)) {
			commands->load_scene(GameplayScene::NAME);
		}
	}

	void MainMenu::draw(engine::Renderer* renderer) const {
		renderer->draw_text(engine::DEFAULT_FONT_ID, 16, { 0, 0 }, engine::RGBA::white(), "MainMenu");
	}

} // namespace game
