#pragma once

#include <filesystem>
#include <functional>
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_set>
#include <variant>
#include <vector>

namespace game {
	struct GameData;
}

namespace engine {

	struct Engine;
	class Scene;
	class Screen;

	class CommandList {
	public:
		void run_commands(Engine* engine);

		/* App */
		void quit();

		/* Input */
		void add_keyboard_binding(std::string action_name, std::unordered_set<uint32_t> keys);

		/* File */
		void load_save_file(std::filesystem::path filepath);
		void write_save_file(std::filesystem::path filepath);

		/* SceneManager */
		void load_scene(std::string scene_name);

		/* ScreenStack */
		void push_screen(std::string screen_name);
		void pop_screen();

		/* Window */
		void toggle_fullscreen();
		void set_window_title(std::string window_title);


	private:
		/* App */
		struct AppCommand_Quit {};

		/* Input */
		struct InputCommand_AddKeyboardBinding {
			std::string action_name;
			std::unordered_set<uint32_t> keys;
		};

		/* File */
		struct FileCommand_LoadSaveFile {
			std::filesystem::path filepath;
		};
		struct FileCommand_WriteSaveFile {
			std::filesystem::path filepath;
		};

		/* SceneManager */
		struct SceneManagerCommand_LoadScene {
			std::string scene_name;
		};

		/* ScreenStack */
		struct ScreenStackCommand_PushScreen {
			std::string screen_name;
		};
		struct ScreenStackCommand_PopScreen {};

		/* Window */
		struct WindowCommand_ToggleFullscreen {};
		struct WindowCommand_SetWindowTitle {
			std::string window_title;
		};

		using Command = std::variant<
			AppCommand_Quit,

			InputCommand_AddKeyboardBinding,

			FileCommand_LoadSaveFile,
			FileCommand_WriteSaveFile,

			SceneManagerCommand_LoadScene,

			ScreenStackCommand_PushScreen,
			ScreenStackCommand_PopScreen,

			WindowCommand_ToggleFullscreen,
			WindowCommand_SetWindowTitle>;

		std::vector<Command> m_commands;
	};

} // namespace engine
