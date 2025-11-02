#pragma once

#include <engine/input/keyboard_stack.h>

#include <engine/graphics/image_id.h>
#include <engine/graphics/rect.h>
#include <engine/math/vec2.h>
#include <engine/scene/scene.h>

namespace game {

	class GameplayScene : public engine::Scene {
	public:
		static constexpr char NAME[] = "GameplayScene";
		GameplayScene();
		void initialize(engine::ResourceManager* resources, engine::CommandList* commands) override;
		void update(const engine::Input& input, engine::CommandList* commands) override;
		void draw(engine::Renderer* renderer) const override;

	private:
		engine::KeyboardStack m_keyboard_stack;
		engine::Vec2 m_player_pos = {};
		engine::ImageID m_sprite_sheet_id = {};
		engine::Rect m_sprite_sheet_size = {};
	};

} // namespace game
