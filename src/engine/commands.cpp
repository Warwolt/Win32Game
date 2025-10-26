#include <engine/commands.h>

#include <engine/container/match_variant.h>
#include <engine/graphics/window.h>

namespace engine {

	void run_commands(const std::vector<Command>& commands, bool* should_quit, Window* window) {
		for (const Command& command : commands) {
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
	}

} // namespace engine
