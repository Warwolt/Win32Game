#pragma once

#include <engine/scene/scene.h>

#include <engine/math/vec2.h>

namespace game {

	class GameplayScene : public engine::Scene {
	public:
		static constexpr char NAME[] = "GameplayScene";
		void initialize(engine::ResourceManager* resources, engine::CommandList* commands) override;
		void update(const engine::Input& input, engine::CommandList* commands) override;
		void draw(engine::Renderer* renderer) const override;

	private:
		std::vector<int> m_input_stack;
		engine::Vec2 m_player_pos;
	};

} // namespace game
