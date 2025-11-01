#include <game/scene/gameplay_scene.h>

#include <engine/commands.h>
#include <engine/graphics/renderer.h>
#include <engine/input/input.h>

#include <game/scene/menu_scene.h>

#include <windows.h>

namespace game {

	void GameplayScene::initialize(engine::ResourceManager* resources, engine::CommandList* /*commands*/) {
	}

	void GameplayScene::update(const engine::Input& input, engine::CommandList* commands) {
		if (input.keyboard.key_was_pressed_now(VK_ESCAPE)) {
			commands->load_scene(MenuScene::NAME);
		}

		if (input.keyboard.key_is_pressed(VK_LEFT)) {
			m_player_pos.x -= 1;
		}
		if (input.keyboard.key_is_pressed(VK_RIGHT)) {
			m_player_pos.x += 1;
		}
		if (input.keyboard.key_is_pressed(VK_UP)) {
			m_player_pos.y -= 1;
		}
		if (input.keyboard.key_is_pressed(VK_DOWN)) {
			m_player_pos.y += 1;
		}
	}

	void GameplayScene::draw(engine::Renderer* renderer) const {
		renderer->clear_screen(engine::RGBA { 252, 216, 168, 255 });

		engine::IVec2 world_origin = renderer->screen_resolution() / 2;

		constexpr int player_size = 16;
		engine::Rect player_rect = {
			m_player_pos.x - player_size / 2,
			m_player_pos.y - player_size / 2,
			16,
			16,
		};
		renderer->draw_rect_fill(player_rect + world_origin, engine::RGBA::green());
	}

} // namespace game
