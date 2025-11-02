#include <game/scene/gameplay_scene.h>

#include <engine/commands.h>
#include <engine/file/resource_manager.h>
#include <engine/graphics/renderer.h>
#include <engine/input/input.h>

#include <game/scene/menu_scene.h>

#include <windows.h>

namespace game {

	GameplayScene::GameplayScene()
		: m_keyboard_stack({ VK_UP, VK_DOWN, VK_LEFT, VK_RIGHT }) {
	}

	void GameplayScene::initialize(engine::ResourceManager* resources, engine::CommandList* /*commands*/) {
		m_sprite_sheet_id = resources->load_image("assets/image/render_test/sprite_sheet.png");
		const engine::Image& sprite_sheet = resources->image(m_sprite_sheet_id);
		m_sprite_sheet_size.width = sprite_sheet.width;
		m_sprite_sheet_size.height = sprite_sheet.height;
	}

	void GameplayScene::update(const engine::Input& input, engine::CommandList* commands) {
		if (input.keyboard.key_was_pressed_now(VK_ESCAPE)) {
			commands->load_scene(MenuScene::NAME);
		}

		m_keyboard_stack.update(input);
		engine::Vec2 input_vector = {};
		if (std::optional<int> keycode = m_keyboard_stack.top_keycode()) {
			if (keycode.value() == VK_UP) {
				input_vector.y -= 1;
			}
			if (keycode.value() == VK_DOWN) {
				input_vector.y += 1;
			}
			if (keycode.value() == VK_LEFT) {
				input_vector.x -= 1;
			}
			if (keycode.value() == VK_RIGHT) {
				input_vector.x += 1;
			}
		}
		const float player_speed = 100.0f; // pixels per second
		m_player_pos += input.time_delta.in_seconds() * player_speed * input_vector;
	}

	void GameplayScene::draw(engine::Renderer* renderer) const {
		renderer->clear_screen(engine::RGBA { 252, 216, 168, 255 });

		// TODO: render sprite facing direction player is walking

		constexpr int player_size = 16;
		const engine::IVec2 world_origin = renderer->screen_resolution() / 2;
		const engine::IVec2 world_player_pos = {
			world_origin.x + (int)std::round(m_player_pos.x) - player_size / 2,
			world_origin.y + (int)std::round(m_player_pos.y) - player_size / 2,
		};
		engine::Rect player_rect = {
			world_player_pos.x,
			world_player_pos.y,
			player_size,
			player_size,
		};
		renderer->draw_image(m_sprite_sheet_id, world_player_pos);
		renderer->draw_rect(player_rect, engine::RGBA::green());
	}

} // namespace game
