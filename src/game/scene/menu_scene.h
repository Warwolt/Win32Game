#pragma once

#include <engine/scene/scene.h>

namespace game {

	class MenuScene : public engine::Scene {
	public:
		static constexpr char NAME[] = "MenuScene";
		void initialize(engine::ResourceManager* resources, engine::CommandList* commands) override;
		void update(const engine::Input& input, engine::CommandList* commands) override;
		void draw(engine::Renderer* renderer) const override;
	};

} // namespace game
