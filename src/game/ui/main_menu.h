#pragma once

#include <engine/ui/screen.h>

namespace game {

	class MainMenu : public engine::Screen {
	public:
		static constexpr char NAME[] = "MainMenu";
		void update(GameData* game, const engine::Input& input, engine::CommandList* commands) override;
		void draw(const GameData& game, engine::Renderer* renderer) const override;

	private:
		int m_menu_index = 0;
		bool m_save_file_exists = false;
	};

} // namespace game
