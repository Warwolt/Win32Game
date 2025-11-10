#include <engine/commands.h>

#include <engine/container/match_variant.h>
#include <engine/debug/assert.h>
#include <engine/engine.h>
#include <engine/file/file.h>
#include <engine/file/save_file.h>
#include <engine/graphics/window.h>
#include <engine/scene/scene_manager.h>
#include <engine/ui/screen_stack.h>

#include <game/game.h>

namespace engine {

	void CommandList::run_commands(Engine* engine) {
		for (size_t i = 0; i < m_commands.size(); i++) {
			const Command& command = m_commands[i];
			MATCH_VARIANT(command) {
				/* App */
				MATCH_CASE0(AppCommand_Quit) {
					engine->should_quit = true;
				}

				/* Input */
				MATCH_CASE(InputCommand_AddKeyboardBinding, action_name, keys) {
					engine->input.bindings.add_keyboard_binding(action_name, keys);
				}

				/* File */
				MATCH_CASE(FileCommand_WriteSaveFile, filepath) {
					/* Write file */
					SaveFile save_file = game::on_write_save_file(*engine->game_data);
					bool did_write = write_string_to_file(save_file.to_json_string(), filepath);
					DEBUG_ASSERT(did_write, "Couldn't open file \"%s\" when writing save file", filepath.string().c_str());
					LOG_INFO("Wrote save file \"%s\"", filepath.string().c_str());
				}

				MATCH_CASE(FileCommand_LoadSaveFile, filepath) {
					/* Read file */
					std::optional<std::string> file_content = read_string_from_file(filepath);
					DEBUG_ASSERT(file_content.has_value(), "Couldn't read save file \"%s\"", filepath.string().c_str());

					/* Parse save data */
					std::expected<SaveFile, SaveFileError> save_file = SaveFile::from_json_string(file_content.value());
					DEBUG_ASSERT(save_file.has_value(), "Save file \"%s\" did not contain valid json", filepath.string().c_str());

					/* Send event */
					game::on_save_file_loaded(engine->game_data, save_file.value());
					LOG_INFO("Loaded save file \"%s\"", filepath.string().c_str());
				}

				/* SceneManager */
				MATCH_CASE(SceneManagerCommand_RegisterScene, scene_name, scene_constructor) {
					engine->scene_manager.register_scene(scene_name, scene_constructor);
				}

				MATCH_CASE(SceneManagerCommand_LoadScene, scene_name) {
					std::optional<SceneManagerError> load_error = engine->scene_manager.load_scene(scene_name);
					DEBUG_ASSERT(!load_error.has_value(), "Failed to load scene \"%s\". Is it registered?", scene_name.c_str());
					engine->scene_manager.current_scene()->initialize(engine->game_data, &engine->resources, this);
					engine->screen_stack.clear();
				}

				/* ScreenStack */
				MATCH_CASE(ScreenStackCommand_RegisterScreen, screen_name, screen_constructor) {
					engine->screen_stack.register_screen(screen_name, screen_constructor);
				}

				MATCH_CASE(ScreenStackCommand_PushScreen, screen_name) {
					const bool pushing_onto_empty_stack = engine->screen_stack.top_screen() == nullptr;

					/* Push screen */
					std::optional<ScreenStackError> push_error = engine->screen_stack.push_screen(screen_name);
					DEBUG_ASSERT(!push_error.has_value(), "Failed to push screen \"%s\". Is it registered?", screen_name.c_str());

					/* Notify scene that it's being paused */
					engine->screen_stack.top_screen()->initialize(engine->game_data, &engine->resources, this);
					if (Scene* current_scene = engine->scene_manager.current_scene()) {
						if (pushing_onto_empty_stack) {
							current_scene->on_pause();
						}
					}
				}

				MATCH_CASE0(ScreenStackCommand_PopScreen) {
					/* Pop screen */
					engine->screen_stack.pop_screen();

					/* Notify scene that it's being unpaused */
					const bool last_screen_popped = engine->screen_stack.top_screen() == nullptr;
					if (Scene* current_scene = engine->scene_manager.current_scene()) {
						if (last_screen_popped) {
							current_scene->on_unpause();
						}
					}
				}

				/* Window */
				MATCH_CASE0(WindowCommand_ToggleFullscreen) {
					engine->window.toggle_fullscreen();
				}

				MATCH_CASE(WindowCommand_SetWindowTitle, window_title) {
					engine->window.set_title(window_title);
				}
			}
		}
		m_commands.clear();
	}

	void CommandList::quit() {
		m_commands.push_back(AppCommand_Quit {});
	}

	void engine::CommandList::add_keyboard_binding(std::string action_name, std::unordered_set<uint32_t> keys) {
		m_commands.push_back(InputCommand_AddKeyboardBinding { action_name, keys });
	}

	void CommandList::load_save_file(std::filesystem::path filepath) {
		m_commands.push_back(FileCommand_LoadSaveFile { filepath });
	}

	void CommandList::write_save_file(std::filesystem::path filepath) {
		m_commands.push_back(FileCommand_WriteSaveFile { filepath });
	}

	void CommandList::toggle_fullscreen() {
		m_commands.push_back(WindowCommand_ToggleFullscreen {});
	}

	void CommandList::set_window_title(std::string window_title) {
		m_commands.push_back(WindowCommand_SetWindowTitle { window_title });
	}

	void CommandList::load_scene(std::string scene_name) {
		m_commands.push_back(SceneManagerCommand_LoadScene { scene_name });
	}

	void CommandList::push_screen(std::string screen_name) {
		m_commands.push_back(ScreenStackCommand_PushScreen { screen_name });
	}

	void CommandList::pop_screen() {
		m_commands.push_back(ScreenStackCommand_PopScreen {});
	}

} // namespace engine
