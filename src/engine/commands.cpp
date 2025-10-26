#include <engine/commands.h>

#include <engine/container/match_variant.h>
#include <engine/graphics/window.h>

namespace engine {

	void CommandList::run(bool* should_quit, Window* window) {
		for (const Command& command : m_commands) {
			/* App commands */
			MATCH_VARIANT_IF(command, AppCommand) {
				MATCH_UNIT_CASE(AppCommand_Quit) {
					*should_quit = true;
				}
				MATCH_UNIT_CASE(AppCommand_ToggleFullscreen) {
					window->toggle_fullscreen();
				}
				MATCH_CASE(AppCommand_SetWindowTitle, window_title) {
					window->set_title(window_title);
				}
			}
		}
		m_commands.clear();
	}

	void CommandList::quit() {
		m_commands.push_back(AppCommand_Quit {});
	}

	void CommandList::toggle_fullscreen() {
		m_commands.push_back(AppCommand_ToggleFullscreen {});
	}

	void CommandList::set_window_title(std::string window_title) {
		m_commands.push_back(AppCommand_SetWindowTitle { window_title });
	}

} // namespace engine
