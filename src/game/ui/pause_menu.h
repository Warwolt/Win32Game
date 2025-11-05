#pragma once

#include <engine/ui/screen.h>

namespace game {

	class PauseMenu : public engine::Screen {
	public:
		static constexpr char NAME[] = "PauseMenu";
		void update(const engine::Input& input, engine::CommandList* commands) override;
		void draw(engine::Renderer* renderer) const override;

	private:
		int m_menu_index = 0;
	};

} // namespace game
