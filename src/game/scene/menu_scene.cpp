#include <game/scene/menu_scene.h>

#include <engine/commands.h>
#include <engine/graphics/renderer.h>
#include <engine/input/input.h>

#include <game/scene/gameplay_scene.h>

#include <windows.h>

namespace game {

	void MenuScene::initialize(engine::ResourceManager* /*resources*/, engine::CommandList* commands) {
		// commands->push_screen(MainMenu::NAME);
	}

	void MenuScene::update(const engine::Input& input, engine::CommandList* commands) {
		if (input.keyboard.key_was_pressed_now(VK_ESCAPE)) {
			commands->quit();
		}

		if (input.keyboard.key_was_pressed_now(VK_RETURN)) {
			commands->load_scene(GameplayScene::NAME);
		}
	}

	void MenuScene::draw(engine::Renderer* renderer) const {
		renderer->draw_text(engine::DEFAULT_FONT_ID, 16, { 0, 16 }, engine::RGBA::white(), "MenuScene");
	}

} // namespace game
