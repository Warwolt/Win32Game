#pragma once

#include <engine/scene/scene.h>

namespace game {

	class MenuScene : public engine::Scene {
	public:
		static constexpr char NAME[] = "MenuScene";
		void initialize(GameData* game, engine::ResourceManager* resources, engine::CommandList* commands) override;
		void update(GameData* game, const engine::Input& input, engine::CommandList* commands) override;
		void draw(const GameData& game, engine::Renderer* renderer) const override;
	};

} // namespace game
