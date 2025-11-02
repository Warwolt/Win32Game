#pragma once

#include <engine/input/keyboard_stack.h>

#include <engine/scene/scene.h>
#include <engine/math/vec2.h>

#include <optional>

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
	};

} // namespace game
