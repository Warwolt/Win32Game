#include <engine/commands.h>

#include <engine/container/match_variant.h>
#include <engine/debug/assert.h>
#include <engine/file/resource_manager.h>
#include <engine/graphics/window.h>
#include <engine/scene/scene_manager.h>
#include <engine/ui/screen_stack.h>

namespace engine {

	void CommandList::run_commands(
		bool* should_quit,
		Window* window,
		ResourceManager* resources,
		SceneManager* scene_manager,
		ScreenStack* screen_stack
	) {
		for (const Command& command : m_commands) {
			MATCH_VARIANT(command) {
				MATCH_CASE0(Command_Quit) {
					*should_quit = true;
				}
				MATCH_CASE0(Command_ToggleFullscreen) {
					window->toggle_fullscreen();
				}
				MATCH_CASE(Command_SetWindowTitle, window_title) {
					window->set_title(window_title);
				}
				MATCH_CASE(Command_LoadScene, scene_name) {
					DEBUG_ASSERT(scene_manager->load_scene(scene_name).has_value(), "Failed to load scene \"%s\"", scene_name.c_str());
					screen_stack->clear();
				}
				MATCH_CASE(Command_PushScreen, screen_name) {
					DEBUG_ASSERT(screen_stack->push_screen(screen_name).has_value(), "Failed to push screen \"%s\"", screen_name.c_str());
					screen_stack->top_screen()->initialize(resources, this);
				}
				MATCH_CASE0(Command_PopScreen) {
					screen_stack->pop_screen();
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

	void CommandList::push_screen(std::string screen_name) {
		m_commands.push_back(Command_PushScreen { screen_name });
	}

	void CommandList::pop_screen() {
		m_commands.push_back(Command_PopScreen {});
	}

} // namespace engine
