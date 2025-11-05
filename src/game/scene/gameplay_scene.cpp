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
		m_walk_animations[Direction::Up] = m_animation_library.add_animation(
			{
				{ SpriteAnimation { .clip = engine::Rect { player_size * 4, 0, player_size, player_size }, .flip_h = false }, frame_duration },
				{ SpriteAnimation { .clip = engine::Rect { player_size * 5, 0, player_size, player_size }, .flip_h = false }, frame_duration },
			},
			{
				.looping = true,
			}
		);
		m_walk_animations[Direction::Down] = m_animation_library.add_animation(
			{
				{ SpriteAnimation { .clip = engine::Rect { player_size * 0, 0, player_size, player_size }, .flip_h = false }, frame_duration },
				{ SpriteAnimation { .clip = engine::Rect { player_size * 1, 0, player_size, player_size }, .flip_h = false }, frame_duration },
			},
			{
				.looping = true,
			}
		);
		m_walk_animations[Direction::Left] = m_animation_library.add_animation(
			{
				{ SpriteAnimation { .clip = engine::Rect { player_size * 2, 0, player_size, player_size }, .flip_h = true }, frame_duration },
				{ SpriteAnimation { .clip = engine::Rect { player_size * 3, 0, player_size, player_size }, .flip_h = true }, frame_duration },
			},
			{
				.looping = true,
			}
		);
		m_walk_animations[Direction::Right] = m_animation_library.add_animation(
			{
				{ SpriteAnimation { .clip = engine::Rect { player_size * 2, 0, player_size, player_size }, .flip_h = false }, frame_duration },
				{ SpriteAnimation { .clip = engine::Rect { player_size * 3, 0, player_size, player_size }, .flip_h = false }, frame_duration },
			},
			{
				.looping = true,
			}
		);
		std::optional<engine::AnimationError> error = m_animation_player.play(m_animation_library, m_walk_animations[Direction::Down], engine::Time::now());
		m_animation_player.pause();
		DEBUG_ASSERT(!error.has_value(), "Couldn't start animation");
	}

	void GameplayScene::update(const engine::Input& input, engine::CommandList* commands) {
		/* Quit to menu */
		if (input.keyboard.key_was_pressed_now(VK_ESCAPE)) {
			commands->load_scene(MenuScene::NAME);
		}

		/* Movement */
		m_keyboard_stack.update(input);
		engine::Vec2 input_vector = { 0, 0 };
		bool just_changed_direction = false;
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
			just_changed_direction = m_player_dir != direction;
			m_player_dir = direction;
		}

		const float player_speed = 75.0f; // pixels per second
		const engine::Vec2 player_velocity = player_speed * input_vector;
		const bool just_changed_velocity = player_velocity != m_player_velocity;
		m_player_velocity = player_velocity;
		m_player_position += input.time_delta.in_seconds() * m_player_velocity;

		// FIXME: we want a "skip_to_next_frame" method on AnimationPlayer I think
		//
		// That way we can make the walk look good in the vertical direction,
		// right now link always stops on the left foot, we want each keyboard
		// press to alterante which foot he rests on.
		//
		// But, to implement that we need to refactor AnimationPlayer to keep
		// track of which frame index it's on as a member, and not just compute
		// that in the `update` and `set_frame` methods.

		if (just_changed_velocity) {
			/* Stop walking animation */
			if (m_player_velocity.length() == 0) {
				m_animation_player.pause();
				DEBUG_ASSERT(!m_animation_player.set_frame(m_animation_library, input.time_now, 0).has_value(), "Couldn't set walk animation frame");
			}
			/* Start walking animation */
			else {
				DEBUG_ASSERT(!m_animation_player.play(m_animation_library, m_walk_animations[m_player_dir], input.time_now).has_value(), "Couldn't play walk animation");
				DEBUG_ASSERT(!m_animation_player.set_frame(m_animation_library, input.time_now, 1).has_value(), "Couldn't set walk animation frame");
			}
		}

		/* Animation */
		m_animation_player.update(m_animation_library, input.time_now);
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

		SpriteAnimation player_animation = m_animation_player.value();
		renderer->draw_image(m_sprite_sheet_id, world_player_pos, { .clip = player_animation.clip, .flip_h = player_animation.flip_h });
	}

} // namespace game
