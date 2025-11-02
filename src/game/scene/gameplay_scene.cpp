#include <game/scene/gameplay_scene.h>

#include <engine/commands.h>
#include <engine/debug/assert.h>
#include <engine/file/resource_manager.h>
#include <engine/graphics/renderer.h>
#include <engine/input/input.h>

#include <game/scene/menu_scene.h>

#include <windows.h>

namespace game {

	using namespace std::chrono_literals;

	constexpr engine::AnimationEntityID PLAYER_ID = engine::AnimationEntityID(1);

	GameplayScene::GameplayScene()
		: m_keyboard_stack({ VK_UP, VK_DOWN, VK_LEFT, VK_RIGHT }) {
	}

	void GameplayScene::initialize(engine::ResourceManager* resources, engine::CommandList* /*commands*/) {
		m_sprite_sheet_id = resources->load_image("assets/image/render_test/sprite_sheet.png");
		const engine::Image& sprite_sheet = resources->image(m_sprite_sheet_id);
		m_sprite_sheet_size.width = sprite_sheet.width;
		m_sprite_sheet_size.height = sprite_sheet.height;

		// set up animations
		const engine::Time frame_duration = 200ms;
		const int player_size = 16;
		m_walk_animations[Direction::Up] = m_sprite_animation_system.add_animation(
			{
				{ SpriteAnimation { .clip = engine::Rect { player_size * 4, 0, player_size, player_size }, .flip_h = false }, frame_duration },
				{ SpriteAnimation { .clip = engine::Rect { player_size * 5, 0, player_size, player_size }, .flip_h = false }, frame_duration },
			},
			{
				.looping = true,
			}
		);
		m_walk_animations[Direction::Down] = m_sprite_animation_system.add_animation(
			{
				{ SpriteAnimation { .clip = engine::Rect { player_size * 0, 0, player_size, player_size }, .flip_h = false }, frame_duration },
				{ SpriteAnimation { .clip = engine::Rect { player_size * 1, 0, player_size, player_size }, .flip_h = false }, frame_duration },
			},
			{
				.looping = true,
			}
		);
		m_walk_animations[Direction::Left] = m_sprite_animation_system.add_animation(
			{
				{ SpriteAnimation { .clip = engine::Rect { player_size * 2, 0, player_size, player_size }, .flip_h = true }, frame_duration },
				{ SpriteAnimation { .clip = engine::Rect { player_size * 3, 0, player_size, player_size }, .flip_h = true }, frame_duration },
			},
			{
				.looping = true,
			}
		);
		m_walk_animations[Direction::Right] = m_sprite_animation_system.add_animation(
			{
				{ SpriteAnimation { .clip = engine::Rect { player_size * 2, 0, player_size, player_size }, .flip_h = false }, frame_duration },
				{ SpriteAnimation { .clip = engine::Rect { player_size * 3, 0, player_size, player_size }, .flip_h = false }, frame_duration },
			},
			{
				.looping = true,
			}
		);
		// FIXME: should we really be grabbing engine::Time::now ? Can we pass in the Input struct here? Should we?
		// FIXME: starting and stopping animation immediately so that player stands still. Can this be expressed more nicely?
		auto _ = m_sprite_animation_system.start_animation(PLAYER_ID, m_walk_animations[Direction::Down], engine::Time::now());
		m_sprite_animation_system.stop_animation(PLAYER_ID);
	}

	void GameplayScene::update(const engine::Input& input, engine::CommandList* commands) {
		/* Quit to menu */
		if (input.keyboard.key_was_pressed_now(VK_ESCAPE)) {
			commands->load_scene(MenuScene::NAME);
		}

		/* Movement */
		m_keyboard_stack.update(input);
		engine::Vec2 input_vector = { 0, 0 };
		if (std::optional<int> keycode = m_keyboard_stack.top_keycode()) {
			Direction direction = {};

			if (keycode.value() == VK_UP) {
				input_vector.y -= 1;
				direction = Direction::Up;
			}
			if (keycode.value() == VK_DOWN) {
				input_vector.y += 1;
				direction = Direction::Down;
			}
			if (keycode.value() == VK_LEFT) {
				input_vector.x -= 1;
				direction = Direction::Left;
			}
			if (keycode.value() == VK_RIGHT) {
				input_vector.x += 1;
				direction = Direction::Right;
			}

			const bool just_changed_direction = direction != m_player_dir;
			m_player_dir = direction;
			if (just_changed_direction || input.keyboard.key_was_pressed_now(keycode.value())) {
				DEBUG_ASSERT(m_sprite_animation_system.start_animation(PLAYER_ID, m_walk_animations[m_player_dir], input.time_now).has_value(), "Missing animation");
				DEBUG_ASSERT(m_sprite_animation_system.set_frame(PLAYER_ID, 1).has_value(), "Shit fuck"); // start at walking frame
			}
		}
		else {
			m_sprite_animation_system.stop_animation(PLAYER_ID);
			auto _ = m_sprite_animation_system.set_frame(PLAYER_ID, 0);
		}

		const float player_speed = 75.0f; // pixels per second
		m_player_velocity = player_speed * input_vector;
		m_player_position += input.time_delta.in_seconds() * m_player_velocity;

		/* Animation */
		m_sprite_animation_system.update(input.time_now);
	}

	void GameplayScene::draw(engine::Renderer* renderer) const {
		renderer->clear_screen(engine::RGBA { 252, 216, 168, 255 });

		constexpr int player_size = 16;
		const engine::IVec2 world_origin = renderer->screen_resolution() / 2;
		const engine::IVec2 world_player_pos = {
			world_origin.x + (int)std::round(m_player_position.x) - player_size / 2,
			world_origin.y + (int)std::round(m_player_position.y) - player_size / 2,
		};
		engine::Rect player_rect = {
			world_player_pos.x,
			world_player_pos.y,
			player_size,
			player_size,
		};

		SpriteAnimation player_animation = m_sprite_animation_system.current_frame(PLAYER_ID);
		renderer->draw_image(m_sprite_sheet_id, world_player_pos, { .clip = player_animation.clip, .flip_h = player_animation.flip_h });
	}

} // namespace game
