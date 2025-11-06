#pragma once

#include <engine/scene/scene.h>

namespace game {

	class MenuScene : public engine::Scene {
	public:
		static constexpr char NAME[] = "MenuScene";
		void initialize(const engine::SaveFile& save_file, engine::ResourceManager* resources, engine::CommandList* commands) override;
		void update(const engine::Input& input, engine::CommandList* commands) override;
		void draw(engine::Renderer* renderer) const override;
	};

} // namespace game
