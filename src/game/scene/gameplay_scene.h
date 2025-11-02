#pragma once

#include <engine/scene/scene.h>

#include <engine/math/vec2.h>

#include <optional>

namespace game {

	class InputStack {
	public:
		InputStack(std::vector<int> keycodes);
		void update(const engine::Input& input);
		std::optional<int> top_keycode() const;

	private:
		std::vector<int> m_keycodes;
		std::vector<int> m_stack;
	};

	class GameplayScene : public engine::Scene {
	public:
		static constexpr char NAME[] = "GameplayScene";
		GameplayScene();
		void initialize(engine::ResourceManager* resources, engine::CommandList* commands) override;
		void update(const engine::Input& input, engine::CommandList* commands) override;
		void draw(engine::Renderer* renderer) const override;

	private:
		InputStack m_input_stack;
		engine::Vec2 m_player_pos = {};
	};

} // namespace game
