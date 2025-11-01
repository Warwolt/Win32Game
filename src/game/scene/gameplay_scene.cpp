#include <game/scene/gameplay_scene.h>

#include <engine/commands.h>
#include <engine/graphics/renderer.h>
#include <engine/input/input.h>

#include <game/scene/menu_scene.h>

#include <windows.h>

namespace game {

	void GameplayScene::update(const engine::Input& input, engine::CommandList* commands) {
		if (input.keyboard.key_was_pressed_now(VK_ESCAPE)) {
			commands->load_scene(MenuScene::NAME);
		}
	}

	void GameplayScene::draw(engine::Renderer* renderer) const {
		renderer->draw_text(engine::DEFAULT_FONT_ID, 16, { 0, 0 }, engine::RGBA::white(), "Gameplay Scene");
	}

} // namespace game
