#pragma once

#include <engine/input/keyboard_stack.h>

#include <engine/animation/animation_system_OLD.h>
#include <engine/graphics/image_id.h>
#include <engine/graphics/rect.h>
#include <engine/math/vec2.h>
#include <engine/scene/scene.h>

#include <unordered_map>

namespace game {

	class GameplayScene : public engine::Scene {
	public:
		static constexpr char NAME[] = "GameplayScene";
		GameplayScene();
		void initialize(engine::ResourceManager* resources, engine::CommandList* commands) override;
		void update(const engine::Input& input, engine::CommandList* commands) override;
		void draw(engine::Renderer* renderer) const override;

	private:
		enum class Direction {
			Up,
			Down,
			Left,
			Right,
		};
		struct SpriteAnimation {
			engine::Rect clip;
			bool flip_h;
		};

		// Movement
		engine::KeyboardStack m_keyboard_stack;
		engine::Vec2 m_player_position = {};
		engine::Vec2 m_player_velocity = {};
		Direction m_player_dir = Direction::Down;

		// Animation
		engine::AnimationSystem_OLD<SpriteAnimation> m_sprite_animation_system;
		std::unordered_map<Direction, engine::AnimationID> m_walk_animations;
		engine::ImageID m_sprite_sheet_id = {};
		engine::Rect m_sprite_sheet_size = {};
	};

} // namespace game
