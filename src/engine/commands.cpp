#include <engine/commands.h>

#include <engine/container/match_variant.h>
#include <engine/graphics/window.h>
#include <engine/audio/audio_player.h>

namespace engine {

	void run_commands(const std::vector<Command>& commands, bool* should_quit, Window* window, AudioPlayer* audio) {
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
			/* Audio commands */
			MATCH_VARIANT_IF(command, AudioCommand) {
				MATCH_CASE(AudioCommand_PlaySound, sound_id) {
					audio->play(sound_id);
				}
			}
		}
	}

} // namespace engine
