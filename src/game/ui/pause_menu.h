#pragma once

#include <engine/input/time.h>
#include <engine/ui/screen.h>

namespace game {

	class PauseMenu : public engine::Screen {
	public:
		static constexpr char NAME[] = "PauseMenu";
		void update(GameData* game, const engine::Input& input, engine::CommandList* commands) override;
		void draw(const GameData& game, engine::Renderer* renderer) const override;

	private:
		enum class State {
			ShowPauseMenu,
			ShowSaveConfirmation,
		};

		int m_menu_index = 0;
		engine::Time m_last_save = {};
		State m_state = State::ShowPauseMenu;
	};

} // namespace game
