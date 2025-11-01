#include <engine/commands.h>

#include <engine/container/match_variant.h>
#include <engine/file/resource_manager.h>
#include <engine/graphics/window.h>
#include <engine/scene/scene_manager.h>
#include <engine/debug/assert.h>

namespace engine {

	void CommandList::run_commands(bool* should_quit, Window* window, ResourceManager* resources, SceneManager* scene_manager) {
		for (const Command& command : m_commands) {
			/* App commands */
			MATCH_VARIANT(command) {
				MATCH_UNIT_CASE(Command_Quit) {
					*should_quit = true;
				}
				MATCH_UNIT_CASE(Command_ToggleFullscreen) {
					window->toggle_fullscreen();
				}
				MATCH_CASE(Command_SetWindowTitle, window_title) {
					window->set_title(window_title);
				}
				MATCH_CASE(Command_LoadScene, scene_name) {
					std::expected<void, SceneManagerError> load_scene_result = scene_manager->load_scene(scene_name);
					DEBUG_ASSERT(load_scene_result.has_value(), "Failed to load scene \"%s\"", scene_name.c_str());
				}
			}
		}
		m_commands.clear();
	}

	void CommandList::quit() {
		m_commands.push_back(Command_Quit {});
	}

	void CommandList::toggle_fullscreen() {
		m_commands.push_back(Command_ToggleFullscreen {});
	}

	void CommandList::set_window_title(std::string window_title) {
		m_commands.push_back(Command_SetWindowTitle { window_title });
	}

	void CommandList::load_scene(std::string scene_name) {
		m_commands.push_back(Command_LoadScene { scene_name });
	}

} // namespace engine
