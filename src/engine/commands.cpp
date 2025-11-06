#include <engine/commands.h>

#include <engine/container/match_variant.h>
#include <engine/debug/assert.h>
#include <engine/file/file.h>
#include <engine/file/resource_manager.h>
#include <engine/file/save_file.h>
#include <engine/graphics/window.h>
#include <engine/scene/scene_manager.h>
#include <engine/ui/screen_stack.h>

namespace engine {

	void CommandList::run_commands(
		bool* should_quit,
		SaveFile* save_file,
		ResourceManager* resources,
		SceneManager* scene_manager,
		ScreenStack* screen_stack,
		Window* window
	) {
		for (size_t i = 0; i < m_commands.size(); i++) {
			const Command& command = m_commands[i];
			MATCH_VARIANT(command) {
				/* App */
				MATCH_CASE0(Command_Quit) {
					*should_quit = true;
				}

				/* File */
				MATCH_CASE(Command_WriteSaveFile, filepath) {
					/* Write file */
					bool did_write = write_string_to_file(save_file->to_json_string(), filepath);
					DEBUG_ASSERT(did_write, "Couldn't open file %s when writing save file", filepath.string().c_str());
				}

				MATCH_CASE(Command_LoadSaveFile, filepath) {
					/* Read file */
					std::optional<std::string> file_content = read_string_from_file(filepath);
					DEBUG_ASSERT(file_content.has_value(), "Couldn't read save file %s", filepath.string().c_str());

					/* Parse save data */
					std::expected<SaveFile, SaveFileError> result = SaveFile::from_json_string(file_content.value());
					DEBUG_ASSERT(result.has_value(), "Save file %s did not contain valid json", filepath.string().c_str());
					*save_file = result.value();

					/* Send event */
					if (Scene* current_scene = scene_manager->current_scene()) {
						current_scene->on_save_file_loaded(*save_file);
					}
				}

				/* SceneManager */
				MATCH_CASE(Command_RegisterScene, scene_name, scene_constructor) {
					scene_manager->register_scene(scene_name, scene_constructor);
				}

				MATCH_CASE(Command_LoadScene, scene_name) {
					std::optional<SceneManagerError> load_error = scene_manager->load_scene(scene_name);
					DEBUG_ASSERT(!load_error.has_value(), "Failed to load scene \"%s\". Is it registered?", scene_name.c_str());
					scene_manager->current_scene()->initialize(*save_file, resources, this);
					screen_stack->clear();
				}

				/* ScreenStack */
				MATCH_CASE(Command_RegisterScreen, screen_name, screen_constructor) {
					screen_stack->register_screen(screen_name, screen_constructor);
				}

				MATCH_CASE(Command_PushScreen, screen_name) {
					const bool pushing_onto_empty_stack = screen_stack->top_screen() == nullptr;

					/* Push screen */
					std::optional<ScreenStackError> push_error = screen_stack->push_screen(screen_name);
					DEBUG_ASSERT(!push_error.has_value(), "Failed to push screen \"%s\". Is it registered?", screen_name.c_str());

					/* Notify scene that it's being paused */
					screen_stack->top_screen()->initialize(resources, this);
					if (Scene* current_scene = scene_manager->current_scene()) {
						if (pushing_onto_empty_stack) {
							current_scene->on_pause();
						}
					}
				}

				MATCH_CASE0(Command_PopScreen) {
					/* Pop screen */
					screen_stack->pop_screen();

					/* Notify scene that it's being unpaused */
					const bool last_screen_popped = screen_stack->top_screen() == nullptr;
					if (Scene* current_scene = scene_manager->current_scene()) {
						if (last_screen_popped) {
							current_scene->on_unpause();
						}
					}
				}

				/* Window */
				MATCH_CASE0(Command_ToggleFullscreen) {
					window->toggle_fullscreen();
				}

				MATCH_CASE(Command_SetWindowTitle, window_title) {
					window->set_title(window_title);
				}
			}
		}
		m_commands.clear();
	}

	void CommandList::quit() {
		m_commands.push_back(Command_Quit {});
	}

	void CommandList::load_save_file(std::filesystem::path filepath) {
		m_commands.push_back(Command_LoadSaveFile { filepath });
	}

	void CommandList::write_save_file(std::filesystem::path filepath) {
		m_commands.push_back(Command_WriteSaveFile { filepath });
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
