#pragma once

#include <engine/ui/screen.h>

namespace game {

	class MainMenu : public engine::Screen {
	public:
		static constexpr char NAME[] = "MainMenu";
		void initialize(engine::ResourceManager* /*resources*/, engine::CommandList* /*commands*/) override;
		void update(const engine::Input& input, engine::CommandList* commands) override;
		void draw(engine::Renderer* renderer) const override;

	private:
		int m_menu_index = 0;
	};

} // namespace game
