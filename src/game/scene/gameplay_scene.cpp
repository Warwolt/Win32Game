#include <game/scene/gameplay_scene.h>

#include <game/game_data.h>
#include <game/ui/pause_menu.h>

#include <engine/commands.h>
#include <engine/debug/assert.h>
#include <engine/file/resource_manager.h>
#include <engine/graphics/renderer.h>
#include <engine/input/input.h>

#include <windows.h>

namespace game {

	using namespace std::chrono_literals;

	static std::unordered_map<Direction, engine::AnimationID> setup_walk_animations(engine::AnimationLibrary<SpriteAnimation>* animation_library) {
		const engine::Time frame_duration = 200ms;
		const int player_size = 16;
		std::unordered_map<Direction, engine::AnimationID> walk_animations;
		walk_animations[Direction::Up] = animation_library->add_animation(
			{
				{ SpriteAnimation { .clip = engine::Rect { player_size * 4, 0, player_size, player_size }, .flip_h = false }, frame_duration },
				{ SpriteAnimation { .clip = engine::Rect { player_size * 5, 0, player_size, player_size }, .flip_h = false }, frame_duration },
			},
			{
				.looping = true,
			}
		);
		walk_animations[Direction::Down] = animation_library->add_animation(
			{
				{ SpriteAnimation { .clip = engine::Rect { player_size * 0, 0, player_size, player_size }, .flip_h = false }, frame_duration },
				{ SpriteAnimation { .clip = engine::Rect { player_size * 1, 0, player_size, player_size }, .flip_h = false }, frame_duration },
			},
			{
				.looping = true,
			}
		);
		walk_animations[Direction::Left] = animation_library->add_animation(
			{
				{ SpriteAnimation { .clip = engine::Rect { player_size * 2, 0, player_size, player_size }, .flip_h = true }, frame_duration },
				{ SpriteAnimation { .clip = engine::Rect { player_size * 3, 0, player_size, player_size }, .flip_h = true }, frame_duration },
			},
			{
				.looping = true,
			}
		);
		walk_animations[Direction::Right] = animation_library->add_animation(
			{
				{ SpriteAnimation { .clip = engine::Rect { player_size * 2, 0, player_size, player_size }, .flip_h = false }, frame_duration },
				{ SpriteAnimation { .clip = engine::Rect { player_size * 3, 0, player_size, player_size }, .flip_h = false }, frame_duration },
			},
			{
				.looping = true,
			}
		);
		return walk_animations;
	}

	GameplayScene::GameplayScene()
		: m_keyboard_stack({ VK_UP, VK_DOWN, VK_LEFT, VK_RIGHT }) {
	}

	void GameplayScene::on_pause() {
		m_scene_is_paused = true;
	}

	void GameplayScene::on_unpause() {
		m_scene_is_paused = false;
	}

	void GameplayScene::initialize(GameData* game, engine::ResourceManager* resources, engine::CommandList* /*commands*/) {
		m_sprite_sheet_id = resources->load_image("assets/image/render_test/sprite_sheet.png");
		const engine::Image& sprite_sheet = resources->image(m_sprite_sheet_id);
		m_sprite_sheet_size.width = sprite_sheet.width;
		m_sprite_sheet_size.height = sprite_sheet.height;

		// set up animations
		m_walk_animations = setup_walk_animations(&m_animation_library);
		DEBUG_ASSERT(!m_animation_player.play(m_animation_library, m_walk_animations[game->player_direction], engine::Time::now()), "Couldn't start animation");
		m_animation_player.pause();
	}

	void GameplayScene::update(GameData* game, const engine::Input& input, engine::CommandList* commands) {
		/* Show pause menu */
		if (input.keyboard.key_was_pressed_now(VK_ESCAPE)) {
			commands->push_screen(PauseMenu::NAME);
		}

		/* Quick load & quick save*/
		const std::string quick_save_filepath = "build/quick_save.json";
		if (input.keyboard.key_was_pressed_now(VK_F5)) {
			commands->write_save_file(quick_save_filepath);
		}
		if (input.keyboard.key_was_pressed_now(VK_F9) && std::filesystem::exists(quick_save_filepath)) {
			commands->load_save_file(quick_save_filepath);
		}

		/* Update non-pausable systems */
		m_keyboard_stack.update(input);

		/* Skip remaining update if we're paused */
		if (m_scene_is_paused) {
			return;
		}

		/* Update pausable systems */
		m_animation_player.update(m_animation_library, input.time_now);

		/* Update pausable game logic */
		auto pause_player_animation = [&]() {
			m_animation_player.pause();
		};
		auto play_player_animation = [&](engine::AnimationID animation_id) {
			std::optional<engine::AnimationError> error = m_animation_player.play(m_animation_library, animation_id, input.time_now);
			DEBUG_ASSERT(!error, "Couldn't play animation");
		};
		auto set_player_animation_frame = [&](int frame) {
			std::optional<engine::AnimationError> error = m_animation_player.set_frame(m_animation_library, input.time_now, frame);
			DEBUG_ASSERT(!error, "Couldn't set frame");
		};
		{
			/* Movement */
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
				just_changed_direction = game->player_direction != direction;
				game->player_direction = direction;
			}

			const float player_speed = 75.0f; // pixels per second
			const engine::Vec2 player_velocity = player_speed * input_vector;
			const bool just_changed_velocity = player_velocity != m_player_velocity;
			m_player_velocity = player_velocity;
			game->player_position += input.time_delta.in_seconds() * m_player_velocity;

			/* Stop walking animation */
			if (m_player_velocity.is_zero()) {
				play_player_animation(m_walk_animations[game->player_direction]);
				pause_player_animation();
				set_player_animation_frame(0);
			}
			if (just_changed_velocity) {
				/* Start walking animation */
				play_player_animation(m_walk_animations[game->player_direction]);
				set_player_animation_frame(1);
			}
		}
	}

	void GameplayScene::draw(const GameData& game, engine::Renderer* renderer) const {
		renderer->clear_screen(engine::RGBA { 252, 216, 168, 255 });

		constexpr int player_size = 16;
		const engine::IVec2 world_origin = renderer->screen_resolution() / 2;
		const engine::IVec2 world_player_pos = {
			world_origin.x + (int)std::round(game.player_position.x) - player_size / 2,
			world_origin.y + (int)std::round(game.player_position.y) - player_size / 2,
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
